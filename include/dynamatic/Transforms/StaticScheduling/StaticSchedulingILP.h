
//===- StaticSchedulingILP.h - Static scheduling support -----------*- C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares common infrastructure for static scheduling algorithms.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_TRANSFORMS_STATICSCHEDULING_STATICSCHEDULINGMILP_H
#define DYNAMATIC_TRANSFORMS_STATICSCHEDULING_STATICSCHEDULINGMILP_H

#include "dynamatic/Analysis/NameAnalysis.h"
#include "dynamatic/Dialect/CFX/CFX.h"
#include "dynamatic/Dialect/Handshake/HandshakeOps.h"
#include "dynamatic/Support/ConstraintProgramming/ConstraintProgramming.h"
#include "dynamatic/Support/DynamaticPass.h"
#include "dynamatic/Support/LLVM.h"
#include "dynamatic/Support/MILP.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LogicalResult.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

namespace dynamatic {

// Create class to represent all static scheduling algorithms
class StaticSchedulingAlgorithm
    : public MILP<llvm::MapVector<Operation *, unsigned>> {

public:
  enum Kind { ASAP, ALAP, PIPELINED };

private:
  // LLVM Implementation of rtti functions like dyn_cast<>, isa<> needs these
  // function
  // [START LLVM RTTI prerequisites]
  Kind kind;

public:
  Kind getKind() const { return kind; }
  static inline bool classof(StaticSchedulingAlgorithm const *) { return true; }
  // [END LLVM RTTI prerequisites]

public:
  // Create MILP formulation for static scheduling
  virtual mlir::LogicalResult
  createFormulation(mlir::ModuleOp moduleOp,
                    const StaticTimingConfig &timingConfig) = 0;
  // Solve the MILP formulation
  mlir::LogicalResult solve(int *milpStatus = nullptr) {
    markReadyToOptimize();
    return optimize(milpStatus);
  }
  // Create optimization objective for the MILP formulation
  virtual mlir::LogicalResult
  createOptimizationObjective(mlir::ModuleOp moduleOp) = 0;
  StaticSchedulingAlgorithm(Kind kind, CPSolver::SolverKind solverKind,
                            int timeout, const std::string &milpName)
      : MILP<llvm::MapVector<Operation *, unsigned>>(solverKind, timeout,
                                                     milpName),
        kind(kind) {}

  // Extract scheduling results from the MILP solution
  void extractResult(llvm::MapVector<Operation *, unsigned> &result) override {
    for (const auto &[op, schedVar] : operationSchedulingVariables) {
      CPVar var = schedVar;
      unsigned schedTime = model->getValue(var);
      result[op] = schedTime;
    }
  }

protected:
  // Create variables and constraints for the MILP formulation
  mlir::LogicalResult createSchedulingVariables(mlir::ModuleOp moduleOp);

  mlir::LogicalResult createDependencyConstraints(mlir::ModuleOp moduleOp,
                                                  const StaticTimingConfig &timingConfig);

  // Function to create resource constraints for the MILP formulation
  mlir::LogicalResult
  createResourceConstraints(mlir::ModuleOp moduleOp,
                            ResourceConstraints &resourceConstraints);

  // Map each operation to its scheduling variables
  llvm::MapVector<Operation *, CPVar> operationSchedulingVariables;
};

} // namespace dynamatic

#endif // DYNAMATIC_TRANSFORMS_STATICSCHEDULING_STATICSCHEDULINGMILP_H
