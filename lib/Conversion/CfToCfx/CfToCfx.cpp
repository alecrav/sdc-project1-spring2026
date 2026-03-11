//===- CfToCfx.cpp ----------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Assumption:
// This comment assumes you understand the purpose of the CFX dialect
// and its phi-based representation of the control-data-flow graph.
//
// This file implements a pass that lowers CF-based control flow into the
// CFX dialect. It first converts the func.func to a cfx.func:
// phi operations cannot be used with func.func due to the software-based
// dominance rules.
// (func.func enforces that inputs are defined before they can be used)
//
// It then inserts a cfx.start, which generates a "control signal"
// (zero-bit integer)
// and replaces all arith.constants with cfx.constants which take this
// zero-bit integer as an input.
// This is used to explicitly represent control-flow dependencies:
// constants should not be used as inputs until the "control" signal
// arrives from a branch.
//
// func.return operations are then transformed into cfx.return operations
// Firstly since func.return operations complain of invalid IR when
// they are not inside a func.func, and we have replaced the func.func.
// Secondly as we pass the zero-bit "control signal" to the cfx.return,
// ensuring the "control signal" remains live throughout the entire circuit.
//
// In normal SSA, when a definition dominates a use, it can be used
// even if the definition and use are in different BBs.
// We instead wish to "maximize" the SSA of the IR.
// This means that when a value is used outside of its defining BB,
// it will pass through explicit cf block arguments.
// As the loop live-ins are therefore 100% explicit, it simplifies
// how we use integer linear programming to schedule pipelined loops.
// We do this using an existing function, inherited from CIRCT.
//
// We then create the cfx branch operations.
// We cannot yet delete the cf branch operations,
// as they are needed to find the block arguments the phi operations replace.
// Therefore, as we create the cfx branch operations,
// we store them in a dictionary so that we can find them later,
// in order to connect them to the phis.
//
// We then add the cfx phi operations, using the above-mentioned dictionary
// to connect them to the correct cfx branch operations.
//
// As cf stores the control-flow graph (CFG) on the branch operations,
// we must copy it to a new representation before we erase the branches,
// so the CFG remains fully present in cfx.
// We do this by informing the cfx.func operation
// of which basic block each operation belongs to.
// This is a diffuse representation, but allows the CFG to be reconstructed
// directly if needed, and is easy to serialize into the IR itself.
// Although we store this information on the individual operations, this is
// an implementation detail. On the logical level, this information is part
// of the cfx.func operation.
//
// We can now remove the cf branch operations,
// as we do not need them after adding the cfx phi operations.
//
// Finally, we merge all of the cf regions into a single graph region
// as cfx uses a single region to represent the circuit.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Dialect/CFX/CFX.h"
#include "dynamatic/Transforms/FuncMaximizeSSA.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

#include "mlir/Dialect/ControlFlow/IR/ControlFlowOps.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"

using namespace mlir;

#include "dynamatic/Conversion/Passes.h"
namespace dynamatic {
#define GEN_PASS_DEF_CFTOCFX
#include "dynamatic/Conversion/Passes.h.inc"
} // namespace dynamatic

namespace dynamatic {

// Skips MemRef-typed block arguments because memrefs are pointer-typed
// and not routed through data-flow branches.
struct CfSSAStrategy : public SSAMaximizationStrategy {
  bool maximizeArgument(BlockArgument arg) override {
    return !isa<MemRefType>(arg.getType());
  }
};

struct CfToCfxPass : public dynamatic::impl::CfToCfxBase<CfToCfxPass> {

  /// Use the auto-generated constructors from tblgen
  using CfToCfxBase::CfToCfxBase;

  // Simple logic to retrieve a cfx branch
  // so we can connect it to the relevant cfx phi
  // Used in two places:
  // 1. Connect cfx branch to cfx phi
  // 2. Or connecting a cfx branch directly to a consumer
  // if a basic block has only one predecessor
  static FailureOr<Value>
  getBranchFromDict(Block *branchBB, Block *succBB, unsigned blockArgIdx,
                    DenseMap<std::tuple<Operation *, unsigned, unsigned>, Value>
                        &findBranchDict) {

    // Get the cf branch
    Operation *term = branchBB->getTerminator();

    // Both branch types share this interface
    auto branch = dyn_cast<BranchOpInterface>(term);
    if (!branch) {
      term->emitError("CfToCfx: predecessor block terminator does not "
                      "implement BranchOpInterface, "
                      "expected cf.br or cf.cond_br");
      return failure();
    }

    // The basic block is either the left or right successor
    for (unsigned s = 0; s < branch->getNumSuccessors(); ++s) {
      if (branch->getSuccessor(s) == succBB) {
        // The key is (cf branch, left or right successor, block arg index)
        auto it = findBranchDict.find({term, s, blockArgIdx});
        if (it == findBranchDict.end()) {
          term->emitError("CfToCfx: no cfx branch found in lookup for "
                          "(predecessor, successor index, block arg index), "
                          "the cf-to-cfx branch conversion may have missed "
                          "an edge");
          return failure();
        }
        return it->second;
      }
    }

    term->emitError("CfToCfx: none of the predecessor's successors match "
                    "the current block, "
                    "control flow graph may be malformed");
    return failure();
  }

  // runDynamaticPass is a wrapper on a ModuleOperation pass
  // which names all operations added to the IR at the end of the pass
  void runDynamaticPass() override {

    // Get the module operation (which represents the entire IR)
    ModuleOp module = getOperation();

    // Add a builder, an MLIR class which allows us to
    // add operations to the IR
    OpBuilder builder(module.getContext());

    // For each func.func in the IR (although there should only be 1)
    // replace it with a cfx.func
    //
    // make_early_inc_range allows me to
    // erase the func ops inside the for loop
    for (auto func :
         llvm::make_early_inc_range(module.getOps<func::FuncOp>())) {

      // Tell the builder where in the IR to add new operations
      // (will crash if not set)
      builder.setInsertionPoint(func);

      // Add a new cfx.func to replace the func.func
      // func.func has dominance rules which phi operations violate
      // So the cfx.func is (hopefully) the same
      // but without those rules
      auto cfxFunc = builder.create<dynamatic::cfx::FuncOp>(
          /*location=*/func.getLoc(),
          /*sym_name=*/builder.getStringAttr(func.getName()),
          /*function_type=*/TypeAttr::get(func.getFunctionType()));

      // Move the function body from func.func to cfx.func
      cfxFunc.getBody().takeBody(func.getBody());

      // Insert cfx.start at the top of the entry block to produce the i0 token.
      Region &region = cfxFunc.getBody();
      builder.setInsertionPointToStart(&region.front());
      Value startVal =
          builder.create<dynamatic::cfx::StartOp>(cfxFunc.getLoc()).getResult();

      // Replace every arith.constant with a cfx.constant gated by startVal.
      for (Block &bb : region) {
        for (auto constOp :
             llvm::make_early_inc_range(bb.getOps<arith::ConstantOp>())) {
          builder.setInsertionPoint(constOp);
          auto cfxConst = builder.create<dynamatic::cfx::ConstantOp>(
              constOp.getLoc(), startVal, constOp.getValue());
          constOp.replaceAllUsesWith(cfxConst.getResult());
          constOp.erase();
        }
      }

      // Convert every func.return to a cfx.return, prepending startVal.
      for (Block &bb : region) {
        for (auto ret :
             llvm::make_early_inc_range(bb.getOps<func::ReturnOp>())) {
          builder.setInsertionPoint(ret);
          builder.create<dynamatic::cfx::ReturnOp>(ret.getLoc(), startVal,
                                                    ret.getOperands());
          ret.erase();
        }
      }

      // remove the func.func
      func.erase();
    }

    // For each function in the IR (although there should only be 1)
    for (auto func : module.getOps<dynamatic::cfx::FuncOp>()) {

      Region &region = func.getBody();

      // In a multi-block function, thread startVal through block arguments so
      // every cfx.constant in a non-entry block is reached via cf.br operands.
      if (region.getBlocks().size() > 1) {
        CfSSAStrategy strategy;
        if (failed(maximizeSSA(region, strategy))) {
          func->emitError("CfToCfx: maximizeSSA failed, "
                          "could not thread values through block arguments");
          return signalPassFailure();
        }
      }

      // ------------------------------------------------------------------
      // Saving the basic block of all operations
      // ------------------------------------------------------------------
      //
      // When we delete the cf operations, we will lose the CFG
      // Before we do that, here we inform the func op
      // which BB each operation belongs to
      // This can then be retrieved during scheduling from the func op


      // Get the first basic block
      Block *entry = &region.front();

      ////////////////////////////////////////////////////////////////////////////
      //                Overview
      ////////////////////////////////////////////////////////////////////////////
      //
      // This section of code aims to convert the cf.br and cf.cond_br
      // operations and their corresponding region block args to cfx.br,
      // cfx.cond_br and cfx.phi operations
      //
      // The CFG is stored on the cf.br and cf.cond_br operations,
      // so these operation cannot be erased until the entire conversion is done
      //
      // To place the cfx.phi operation, we need the correct inputs.
      // Since cf.br and cf.cond_br operations have no outputs,
      // all cf.br and cf.cond_br operations must be converted
      // to cfx.br and cfx.cond_br operations before any cfx.phi can be placed
      //
      // However, we then need a way for a cfx.phi operation to find the
      // cfx.br and cfx.cond_br operations.
      // The findBranchDict allows us to do this, as we store the output of each
      // cfx.br and cfx.cond_br operations for the relevant cfx.phi to retrieve

      DenseMap<std::tuple<Operation *, unsigned, unsigned>, Value>
          findBranchDict;

      // Loop 1: Iterate over every basic block
      // and convert its branches
      for (Block &bb : region) {
        // Get the branch
        Operation *term = bb.getTerminator();

        // Insert new branch where old branch was
        builder.setInsertionPoint(term);

        // Convert cf.br to cfx.br
        if (auto br = dyn_cast<cf::BranchOp>(term)) {
          // Get the n live-outs of this bb
          // All n live-outs are inputs to a single cf.br
          // But we will make 1 cfx.br per live-out
          auto ops = br.getOperands();

          // Iterate over the cf.br operands
          for (unsigned i = 0, e = ops.size(); i < e; ++i) {
            Value op = ops[i];

            // Make a cfx.br
            auto cfx_branch = builder.create<dynamatic::cfx::BranchOp>(
                /*location=*/br.getLoc(),
                /*output type=*/op.getType(),
                /*input=*/op);

            // Store the output of the branch in a dictionary
            // So it can be found when we add the phis
            // Key is (cf.br, left or right successor, operand index)
            // A cf.br has only one successor, so we key it as 0
            findBranchDict[{term, 0, i}] = cfx_branch.getResult();
          }

        // Convert cf.cond_br to cfx.cond_br
        } else if (auto cbr = dyn_cast<cf::CondBranchOp>(term)) {
          // For both the left successor of this bb
          // and the right successor
          for (unsigned s = 0; s < 2; ++s) {
            // Get from the l = 1 + m + n operands of the cf.cond_br
            // i.e.
            // condition_operand + left_succ_operands + right_succ_operands
            // Get either the m operands for the left side successor
            // or n operands for the right side successor
            // The either m or n live outs feed a single cf.cond_br
            // But we will make 1 cfx.cond_br per live-out
            auto ops = cbr.getSuccessorOperands(s);

            // Iterate over the cf.cond_br operands
            for (unsigned i = 0, e = ops.size(); i < e; ++i) {
              Value op = ops[i];

              // There is one conditional branch per operand per successor
              // So we need to store whether this a "true" branch
              // or a "false" branch
              auto polarity = (s == 0)
                                  ? dynamatic::cfx::BranchPolarity::when_true
                                  : dynamatic::cfx::BranchPolarity::when_false;

              // Convert to attr for storing on operation
              auto polarity_attr = dynamatic::cfx::BranchPolarityAttr::get(
                  builder.getContext(), polarity);

              auto cfx_branch = builder.create<dynamatic::cfx::CondBranchOp>(
                  /*location=*/cbr.getLoc(),
                  /*resultType=*/op.getType(),
                  /*cond=*/cbr.getCondition(),
                  /*input=*/op,
                  /*polarity=*/polarity_attr);

              // Store the output of the branch in a dictionary
              // So it can be found when we add the phis
              // Key is (cf.cond_br, left or right successor, operand index)
              findBranchDict[{term, s, i}] = cfx_branch.getResult();
            }
          }
        }
      }

      // Loop 2: Iterate over every basic block
      // and add phis
      for (Block &bb : region) {

        // If 0 predecessors:
        // leave the function block args as they are
        //
        // If 1 predecessor,
        // directly connect branch to consumers
        //
        // If more than 1 predecessor,
        // insert phis

        auto preds = bb.getPredecessors();
        size_t numPreds = std::distance(preds.begin(), preds.end());
        if (numPreds == 0)
          continue; // entry block

        // Insert phis at the start of the block
        builder.setInsertionPointToStart(&bb);

        // For each block argument
        for (BlockArgument arg : bb.getArguments()) {
          unsigned argIdx = arg.getArgNumber();

          // If exactly one predecessor,
          // directly connect branch to consumers
          if (numPreds == 1) {
            Block *pred = *preds.begin();

            // We have only one incoming edge to this block,
            // so no phi is needed.
            // We retrieve the cfx branch output corresponding
            // to this predecessor and block argument
            auto input = getBranchFromDict(
                /*branchBB=*/pred,
                /*succBB=*/&bb,
                /*blockArgIdx=*/argIdx,
                /*findBranchDict=*/findBranchDict);
            if (failed(input))
              return signalPassFailure();

            // Replace all uses of the block argument with the
            // value produced by the cfx branch.
            // Directly connect the branch producer
            // to the block argument consumers
            arg.replaceAllUsesWith(*input);
          } else {

            // Empty vector
            // which we will fill with the inputs to the phi
            SmallVector<Value> phiInputs;

            // Collect one phi input per predecessor
            for (Block *pred : bb.getPredecessors()) {
              // Get the cf branch
              Operation *term = pred->getTerminator();

              // Both branch types share this interface
              auto branch = dyn_cast<BranchOpInterface>(term);
              if (!branch) {
                term->emitError(
                    "CfToCfx: predecessor block terminator does not "
                    "implement BranchOpInterface, "
                    "expected cf.br or cf.cond_br");
                return signalPassFailure();
              }

              // The basic block of the phi is either
              // the left successor or right successor of the branch.
              // We need to know which to find the correct phi input.
              bool found = false;
              for (unsigned s = 0; s < branch->getNumSuccessors(); ++s) {
                if (branch->getSuccessor(s) == &bb) {
                  auto phiInput = getBranchFromDict(
                      /*branchBB=*/pred,
                      /*succBB=*/&bb,
                      /*blockArgIdx=*/argIdx,
                      /*findBranchDict=*/findBranchDict);
                  if (failed(phiInput))
                    return signalPassFailure();

                  phiInputs.push_back(*phiInput);
                  found = true;
                  break;
                }
              }

              if (!found) {
                term->emitError(
                    "CfToCfx: none of the predecessor's successors match "
                    "the current block, "
                    "control flow graph may be malformed");
                return signalPassFailure();
              }
            }

            // Create the phi
            auto phi = builder.create<dynamatic::cfx::PhiOp>(
                /*location=*/bb.getParent()->getLoc(),
                /*phi output type=*/arg.getType(),
                /*operand list=*/phiInputs);

            // Replace the block arg in the IR with the phi
            arg.replaceAllUsesWith(phi.getResult());
          }
        }
      }

      // ------------------------------------------------------------------
      // Saving the basic block of all operations
      // ------------------------------------------------------------------
      //
      // When we delete the cf operations, we will lose the CFG
      // Before we do that, here we inform the func op
      // which BB each operation belongs to
      // This can then be retrieved during scheduling from the func op

      int blockID = 0;
      for (Block &bb : func.getBody()) {
        for (Operation &op : bb) {
          func.setBB(&op, blockID);
        }
        ++blockID;
      }

      // ------------------------------------------------------------------
      // Branch Erasing
      // ------------------------------------------------------------------
      //
      // Now that the cfx operations are all in place, we can delete
      // the cf operations
      //

      for (Block &bb : region) {

        Operation *term = bb.getTerminator();

        if (auto br = dyn_cast<cf::BranchOp>(term)) {
          br.erase();
        } else if (auto cbr = dyn_cast<cf::CondBranchOp>(term)) {
          cbr.erase();
        }
      }

      //-------------------------------------------------------------------
      // Region Merging
      //-------------------------------------------------------------------
      //
      // Here the block args are removed by removing the regions

      for (Block &bb : llvm::make_early_inc_range(llvm::drop_begin(region))) {
        entry->getOperations().splice(entry->end(), bb.getOperations());
        bb.erase();
      }
    }
  }
};

} // namespace dynamatic
