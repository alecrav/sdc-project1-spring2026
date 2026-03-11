//===- CFX.cpp - Implement the CFX dialect -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Dialect/CFX/CFX.h"
#include "dynamatic/Analysis/NameAnalysis.h"
#include "dynamatic/Dialect/Handshake/HandshakeAttributes.h"
#include "dynamatic/Support/Attribute.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/Interfaces/FunctionImplementation.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdint>

using namespace dynamatic;
using namespace dynamatic::handshake;
using namespace cfx;

// Include the dialect definition from the tablegen
#include "dynamatic/Dialect/CFX/CFXDialect.cpp.inc"

// Add the operations and attribute to the dialect
void CFXDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "dynamatic/Dialect/CFX/CFX.cpp.inc"
      >();
  addAttributes<
#define GET_ATTRDEF_LIST
#include "dynamatic/Dialect/CFX/CFXAttributes.cpp.inc"
      >();
}

// Add the operation definitions from the tablegen
#define GET_OP_CLASSES
#include "dynamatic/Dialect/CFX/CFX.cpp.inc"

// Add the enum definitions from the tablegen
#include "dynamatic/Dialect/CFX/CFXEnums.cpp.inc"

#include "llvm/ADT/TypeSwitch.h"
#define GET_ATTRDEF_CLASSES
// Add the attribute definitions from the tablegen
#include "dynamatic/Dialect/CFX/CFXAttributes.cpp.inc"

// Print function arguments as named SSA values in the function signature,
// matching func.func. printFunctionOp and parseFunctionOp are utilities from
// mlir/Interfaces/FunctionImplementation.h shared across all MLIR function-like
// ops that implement FunctionOpInterface.
void FuncOp::print(OpAsmPrinter &p) {
  mlir::function_interface_impl::printFunctionOp(
      p, *this, /*isVariadic=*/false, getFunctionTypeAttrName(),
      getArgAttrsAttrName(), getResAttrsAttrName());
}

ParseResult FuncOp::parse(OpAsmParser &parser, OperationState &result) {
  auto buildFuncType =
      [](Builder &builder, ArrayRef<Type> argTypes, ArrayRef<Type> results,
         mlir::function_interface_impl::VariadicFlag,
         std::string &) { return builder.getFunctionType(argTypes, results); };

  return mlir::function_interface_impl::parseFunctionOp(
      parser, result, /*allowVariadic=*/false,
      getFunctionTypeAttrName(result.name), buildFuncType,
      getArgAttrsAttrName(result.name), getResAttrsAttrName(result.name));
}

// Custom verifier to ensure input and result of
// CondBranchOp are the same type
::mlir::LogicalResult CondBranchOp::verify() {
  if (getInput().getType() != getResult().getType())
    return emitOpError("input and result must have the same type");
  return ::mlir::success();
}

// Hardcoded name for the basic block attribute
// Purposely in CPP file to prevent access to the attribute directly
// The FuncOp functions should be used only
static constexpr const char *bbAttrName = "cfx.bb";

// Ask the FuncOp the bbID of the operation
int64_t FuncOp::getBB(::mlir::Operation *op) {
  if (!op)
    llvm::report_fatal_error("getBB: operation pointer is null");
  auto attr = op->getAttrOfType<::mlir::IntegerAttr>(bbAttrName);
  if (!attr)
    llvm::report_fatal_error("getBB: operation is missing the cfx.bb attribute");
  return attr.getInt();
}

// Tell the FuncOp the bbID of the operation
void FuncOp::setBB(::mlir::Operation *op, int64_t bbID) {
  if (!op)
    llvm::report_fatal_error("setBB: operation pointer is null");
  if (bbID < 0)
    llvm::report_fatal_error("setBB: basic block ID must be non-negative");

  ::mlir::Builder b(op->getContext());
  auto type = b.getIntegerType(64);
  op->setAttr(bbAttrName, b.getIntegerAttr(type, bbID));
}

// Function to get the list of operations separated in basic blocks
llvm::SmallVector<CFXBasicBlock> FuncOp::getCFXBasicBlocks() {
  llvm::SmallVector<CFXBasicBlock> basicBlocks;
  llvm::DenseMap<int64_t, llvm::SmallVector<Operation *>> bbIDToOps;
  if (this->getBlocks().size() != 1)
    llvm::report_fatal_error("getCFXBasicBlocks: cfx.func must have exactly "
                             "one block after region merging");
  // Collect operations in the function and group them by their bbID
  for (mlir::Block &block : this->getBlocks()) {
    for (mlir::Operation &op : block.getOperations()) {
      int64_t bbID = this->getBB(&op);
      bbIDToOps[bbID].push_back(&op);
    }
  }
  // Get the keys of map and sort them to ensure the basic blocks are ordered by
  // their IDs
  llvm::SmallVector<int64_t> bbIDs;
  for (const auto &entry : bbIDToOps) {
    bbIDs.push_back(entry.first);
  }
  llvm::sort(bbIDs);
  int64_t expectedBBID = 0;
  // Collect the operations in vector structure
  for (auto ordKey : bbIDs) {
    auto entry = bbIDToOps.find(ordKey);
    int64_t bbID = entry->first;
    if (bbID != expectedBBID)
      llvm::report_fatal_error("getCFXBasicBlocks: basic block IDs must be "
                               "continuous and start from 0");
    expectedBBID++;
    llvm::SmallVector<Operation *> ops = entry->second;
    basicBlocks.push_back(CFXBasicBlock(bbID, ops));
  }
  return basicBlocks;
}

llvm::SmallVector<CFXBasicBlock::MemoryDependence>
CFXBasicBlock::getMemoryDependencies() const {
  llvm::SmallVector<CFXBasicBlock::MemoryDependence> retVal;

  llvm::StringMap<Operation *> nameToOp;

  // Iterate over ops in the basic block
  for (auto op : *this) {
    if (auto nameAttr =
            op->getAttrOfType<mlir::StringAttr>(NameAnalysis::ATTR_NAME)) {
      nameToOp[nameAttr.str()] = op;
    }
  }

  // Iterate over ops in the basic block
  for (auto op : *this) {
    // All stores involved in a WAR with the load are still dependent
    if (auto deps = getDialectAttr<MemDependenceArrayAttr>(op)) {
      for (MemDependenceAttr dependency : deps.getDependencies()) {
        auto dstOpName = dependency.getDstAccess();
        auto dist = dependency.getDistance();
        if (auto *dstOp = nameToOp.at(dstOpName)) {
          retVal.emplace_back(op, dstOp, dist);
        }
      }
    }
  }

  return retVal;
}

// Get self-loop back edges for this basic block: pairs of (branch, phi)
// where a branch in this BB feeds a phi in the same BB
llvm::SmallVector<CFXBasicBlock::BackEdge>
CFXBasicBlock::getSelfLoopBackEdges() const {
  llvm::SmallVector<BackEdge> backEdges;
  // Iterate over ops in the basic block
  for (auto op : *this) {
    // Only branch and conditional branch ops can have back edges
    if (!mlir::isa<cfx::CondBranchOp>(op) && !mlir::isa<cfx::BranchOp>(op))
      continue;
    // Check each result of the branch for uses by phi ops in this BB
    for (unsigned i = 0; i < op->getNumResults(); ++i) {
      for (auto &use : op->getResult(i).getUses()) {
        Operation *successor = use.getOwner();
        // A phi in the same BB consuming a branch result is a self-loop back edge
        if (mlir::isa<cfx::PhiOp>(successor) && hasOp(successor))
          backEdges.push_back(std::make_pair(op, successor));
      }
    }
  }
  return backEdges;
}

bool CFXBasicBlock::isPipelinable() const {
  return !getSelfLoopBackEdges().empty();
}

// Function to get only the back edges that are self-loops in basic blocks
llvm::SetVector<std::pair<Operation *, Operation *>>
FuncOp::getSelfLoopBackEdgesInCDFG() {
  llvm::SetVector<std::pair<Operation *, Operation *>> selfLoopBackEdges;
  if (this->getBlocks().size() != 1)
    llvm::report_fatal_error("getSelfLoopBackEdgesInCDFG: cfx.func must have "
                             "exactly one block after region merging");
  for (mlir::Block &block : this->getBlocks()) {
    for (mlir::Operation &op : block.getOperations()) {
      // Check the operation is a branch operation cfx::CondBranchOp or
      // cfx::BranchOp
      if (auto condBranchOp = mlir::dyn_cast<cfx::CondBranchOp>(op)) {
        // Get the BB of the cond branch operation
        int64_t branchBBID = this->getBB(&op);
        // If so, check its successors if it is a cfx::PhiOp node
        // Get number of results
        uint32_t numResults = condBranchOp->getNumResults();
        for (unsigned i_succ = 0; i_succ < numResults; ++i_succ) {
          // Get uses of the result
          auto succOp = condBranchOp->getResult(i_succ);
          // Iterate over uses to find defining operations
          for (auto &use : succOp.getUses()) {
            Operation *succDefOp = use.getOwner();
            // Check if the successor operation is a phi node
            if (auto phiOp = mlir::dyn_cast<cfx::PhiOp>(succDefOp)) {
              // If the successor block is the same as the current block, then
              // we have a self-loop back edge
              int64_t phiBBID = this->getBB(phiOp);
              if (phiBBID == branchBBID) {
                // Check if the phi node is in the same block as the branch
                selfLoopBackEdges.insert(std::make_pair(&op, phiOp));
              }
            }
          }
        }
      } else if (auto branchOp = dyn_cast<cfx::BranchOp>(op)) {
        // Get the BB of the branch operation
        int64_t branchBBID = this->getBB(&op);
        // If so, check its successors if it is a cfx::PhiOp node
        // Get number of results
        uint32_t numResults = branchOp->getNumResults();
        for (unsigned i_succ = 0; i_succ < numResults; ++i_succ) {
          // Get uses of the result
          auto succOp = branchOp->getResult(i_succ);
          // Iterate over uses to find defining operations
          for (auto &use : succOp.getUses()) {
            Operation *succDefOp = use.getOwner();
            // Check if the successor operation is a phi node
            if (auto phiOp = dyn_cast<cfx::PhiOp>(succDefOp)) {
              // If the successor block is the same as the current block, then
              // we have a self-loop back edge
              int64_t phiBBID = this->getBB(phiOp);
              if (phiBBID == branchBBID) {
                // Check if the phi node is in the same block as the branch
                selfLoopBackEdges.insert(std::make_pair(&op, phiOp));
              }
            }
          }
        }
      }
    }
  }
  return selfLoopBackEdges;
}

// Define function to uniquely identify BB in the II attribute map
std::string getBBKeyIIMap(uint32_t bbID) {
  // BB is unique identified by its index in the function
  return "BB_" + std::to_string(bbID);
}

// Hardcoded name for the ii map attribute
// Purposely in CPP file to prevent access to the attribute directly
// The FuncOp functions should be used only
static constexpr const char *iiMapAttrName = "cfx.ii_map";

// Function to set the II value for a basic block in the function attribute
void FuncOp::setInitiationIntervalForBlock(uint32_t bbID, unsigned ii) {
  // Read the II dictionary attribute from the func op.
  // Returns nullptr if no II values have been set yet.
  auto iiDictAttr = this->getOperation()->getAttrOfType<mlir::DictionaryAttr>(iiMapAttrName);
  // Check if the attribute already exists
  if (!iiDictAttr || iiDictAttr.empty()) {
    // If not, create the dictionary attribute and set the II value for the
    // block
    mlir::DictionaryAttr iiDictAttr;
    mlir::IntegerAttr iiAttr = mlir::IntegerAttr::get(
        mlir::IntegerType::get(this->getContext(), 32), ii);
    iiDictAttr = mlir::DictionaryAttr::get(
        this->getContext(),
        {{mlir::StringAttr::get(this->getContext(), getBBKeyIIMap(bbID)),
          iiAttr}});
    // Write the updated II dictionary back to the func op.
    this->getOperation()->setAttr(iiMapAttrName, iiDictAttr);
    return;
  }
  // If the attribute already exists, update or add the II value for the block
  // Create the integer attribute for the II value
  mlir::IntegerAttr newIIAttr = mlir::IntegerAttr::get(
      mlir::IntegerType::get(this->getContext(), 32), ii);
  // BB is unique identified by its index in the function
  std::string blockKey = getBBKeyIIMap(bbID);
  // Iterate through existing attributes and update or add the II value
  llvm::SmallVector<mlir::NamedAttribute> newiiAttrs;
  bool found = false;
  for (auto attr : iiDictAttr.getValue()) {
    if (attr.getName() == blockKey) {
      newiiAttrs.push_back(
          {mlir::StringAttr::get(this->getContext(), blockKey), newIIAttr});
      found = true;
    } else {
      newiiAttrs.push_back({attr.getName(), attr.getValue()});
    }
  }
  if (!found) {
    newiiAttrs.push_back(
        {mlir::StringAttr::get(this->getContext(), blockKey), newIIAttr});
  }
  // Write the updated II dictionary back to the func op.
  iiDictAttr = mlir::DictionaryAttr::get(this->getContext(), newiiAttrs);
  this->getOperation()->setAttr(iiMapAttrName, iiDictAttr);
}

// Function to set the II value for a basic block in the function attribute
void FuncOp::setInitiationIntervalForBlock(const CFXBasicBlock &bb,
                                           unsigned ii) {
  setInitiationIntervalForBlock(bb.getID(), ii);
}

// Function to get the II value for basic block pipelined scheduling
std::optional<unsigned> FuncOp::getInitiationIntervalForBlock(uint32_t bbID) {

  // Read the II dictionary attribute from the func op.
  auto iiDictAttr = this->getOperation()->getAttrOfType<mlir::DictionaryAttr>(iiMapAttrName);
  if (!iiDictAttr || iiDictAttr.empty()) {
    return std::nullopt; // No II values stored
  }
  // Extract the list of key-value pairs from the dictionary attribute.
  auto iiAttrsList = iiDictAttr.getValue();
  // BB is unique identified by its index in the function
  std::string blockKey = getBBKeyIIMap(bbID);
  // Each attr is a NamedAttribute, which is a key-value pair.
  // We iterate through them to find the II value for this block.
  for (auto attr : iiAttrsList) {
    if (attr.getName() == blockKey) {
      mlir::IntegerAttr iiAttr = attr.getValue().dyn_cast<mlir::IntegerAttr>();
      if (iiAttr) {
        return static_cast<unsigned>(iiAttr.getInt());
      }
    }
  }
  return std::nullopt;
}

// Define function to get II value for basic block pipelined scheduling
std::optional<unsigned>
FuncOp::getInitiationIntervalForBlock(const CFXBasicBlock &bb) {
  return getInitiationIntervalForBlock(bb.getID());
}

// Function to check if a given operation is in the basic block
bool CFXBasicBlock::hasOp(mlir::Operation *op) const {
  for (auto candidateOp : *this) {
    if (candidateOp == op) {
      return true;
    }
  }
  return false;
}

// Function to get the operations in the basic block in topological order
llvm::SmallVector<mlir::Operation *>
CFXBasicBlock::getTopologicallySortedOps() const {
  llvm::SmallVector<Operation *> topoOrder;
  llvm::SetVector<Operation *> visited;
  // Depth-first search to get topological order
  std::function<void(Operation *)> dfs = [&](Operation *op) {
    visited.insert(op);
    for (auto succ : op->getUsers()) {
      if (!this->hasOp(succ))
        continue;
      if (!visited.contains(succ)) {
        dfs(succ);
      }
    }
    topoOrder.push_back(op);
  };
  // Start DFS from each operation in the block
  for (auto op : *this) {
    if (!visited.contains(op)) {
      dfs(op);
    }
  }
  // Reverse the order to get correct topological order
  std::reverse(topoOrder.begin(), topoOrder.end());
  return topoOrder;
}

// Function to add scheduling variables as attributes to operations
mlir::LogicalResult FuncOp::addSchedulingAttributes(
    llvm::MapVector<Operation *, unsigned> &result) {
  for (auto [op, schedTime] : result) {
    op->setAttr(SchedVarAttr::getMnemonic(),
                SchedVarAttr::get(op->getContext(), schedTime));
  }
  // Check that inside each function all operations have the scheduling
  // attribute
  for (auto &block : getBlocks()) {
    for (Operation &op : block.getOperations()) {
      if (!op.hasAttr(SchedVarAttr::getMnemonic())) {
        emitError() << "Operation " << op
                    << " is missing scheduling attribute after "
                       "static scheduling";
        return failure();
      }
    }
  }
  return success();
}
