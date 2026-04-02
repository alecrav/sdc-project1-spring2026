//===- ASAPScheduling.cpp - ASAP scheduling pass --------*- C++ -*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the ASAP scheduling algorithm.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Transforms/StaticScheduling/ASAPScheduling.h"
#include "dynamatic/Support/ConstraintProgramming/ConstraintProgramming.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"

namespace dynamatic {

// Define the optimization function for ASAPScheduling
mlir::LogicalResult ASAPScheduling::createOptimizationObjective(mlir::ModuleOp moduleOp) {
  // [START Student Assignment]
  LinExpr objective;

  
  this->model->SetMinimizeObjective();
  // [END Student Assignment]
  return success();
}

// Define the createFormulation method for ASAPScheduling
mlir::LogicalResult
ASAPScheduling::createFormulation(mlir::ModuleOp moduleOp,
                                  const StaticTimingConfig &timingConfig) {
  // Create scheduling variables
  if (failed(createSchedulingVariables(moduleOp))) {
    return mlir::failure();
  }
  // Create dependency constraints
  if (failed(createDependencyConstraints(moduleOp, timingConfig))) {
    return mlir::failure();
  }
  // Load and apply resource constraints from the file provided at construction
  if (useResourceConstraints) {
    ResourceConstraints resourceConstraints;
    if (failed(resourceConstraints.readFromJSON(resourceConstraintsFile))) {
      llvm::errs() << "=== Resource constraints read failed ===\n";
      return mlir::failure();
    }
    if (failed(createResourceConstraints(moduleOp, resourceConstraints))) {
      return mlir::failure();
    }
  }
  // Create optimization objective
  if (failed(createOptimizationObjective(moduleOp))) {
    return mlir::failure();
  }

  return mlir::success();
}

} // namespace dynamatic
