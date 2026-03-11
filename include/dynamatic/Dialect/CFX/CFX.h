//===- CFX.h - CFX dialect declaration -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_DIALECT_CFX_CFX_H
#define DYNAMATIC_DIALECT_CFX_CFX_H

#include "dynamatic/Support/LLVM.h"
#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/Region.h"

#include "mlir/Interfaces/InferTypeOpInterface.h"

#include "mlir/IR/RegionKindInterface.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/Interfaces/FunctionInterfaces.h"

#include "dynamatic/Support/DynamaticPass.h"

namespace dynamatic {
namespace detail {
struct CFXBasicBlockImpl {
  /// List of operations
  llvm::SmallVector<mlir::Operation *> ops;
  int64_t bbId;
};
} // namespace detail

class CFXBasicBlock {
  // Reduce excessive memory copying and allocation, and allows us to use
  // CFXBasicBlock as a key
  std::shared_ptr<detail::CFXBasicBlockImpl> impl;

public:
  using MemoryDependence = std::tuple<Operation *, Operation *, int>;
  using BackEdge = std::pair<mlir::Operation *, mlir::Operation *>;
  using const_iterator = llvm::SmallVector<mlir::Operation *>::const_iterator;
  CFXBasicBlock(int64_t bbId, llvm::SmallVector<mlir::Operation *> ops)
      : impl(std::make_shared<detail::CFXBasicBlockImpl>(
            detail::CFXBasicBlockImpl{std::move(ops), bbId})) {}
  bool empty() const { return impl->ops.empty(); }
  int64_t getID() const { return impl->bbId; }
  /// Numeric ID of the basic block
  /// Topologically sort the operations in the bb
  llvm::SmallVector<mlir::Operation *> getTopologicallySortedOps() const;
  /// Check if a given operation is in the BB
  bool hasOp(mlir::Operation *op) const;

  // Get all the memory dependencies, that is, a tuple of (src, dst, dist)
  // within the same BB
  llvm::SmallVector<MemoryDependence> getMemoryDependencies() const;

  // Get self-loop back edges: pairs of (branch op, phi op) where a branch
  // in this BB feeds a phi in the same BB
  llvm::SmallVector<BackEdge> getSelfLoopBackEdges() const;

  // A BB is pipelinable if it has self-loop back edges (branches to itself)
  bool isPipelinable() const;

  /// enables `for (auto op : block)` (only const iterator is allowed):
  const_iterator begin() const { return impl->ops.begin(); }
  const_iterator end() const { return impl->ops.end(); }
};
} // namespace dynamatic

// Include the dialect from the tablegen files
#include "dynamatic/Dialect/CFX/CFXDialect.h.inc"

// Include the enum declarations from the tablegen files
#include "dynamatic/Dialect/CFX/CFXEnums.h.inc"

#define GET_ATTRDEF_CLASSES
// Include the attribute declarations from the tablegen files
#include "dynamatic/Dialect/CFX/CFXAttributes.h.inc"

// Include the operation decls from the tablegen files
#define GET_OP_CLASSES
#include "dynamatic/Dialect/CFX/CFX.h.inc"

#endif // DYNAMATIC_DIALECT_CFX_CFX_H
