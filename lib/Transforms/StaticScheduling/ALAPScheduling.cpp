//===- ALAPScheduling.cpp - ALAP scheduling pass --------*- C++ -*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the ALAP scheduling algorithm.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Transforms/StaticScheduling/ALAPScheduling.h"
#include "dynamatic/Support/ConstraintProgramming/ConstraintProgramming.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"

namespace dynamatic {

// Define the optimization function for ALAPScheduling
mlir::LogicalResult
ALAPScheduling::createOptimizationObjective(mlir::ModuleOp moduleOp) {
  // [START Student Assignment]
  // Check model is created
  if (!model) {
    llvm::errs() << "MILP model is not initialized.\n";
    return mlir::failure();
  }
  LinExpr objective;

  // Create scheduling variables for each operation in the module
  for (auto funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    for (auto &block : funcOp.getCFXBasicBlocks()) {
      for (auto *op : block) {
        CPVar schedVar = this->operationSchedulingVariables.lookup(op);
        objective += schedVar;
      }
    }
  }
  this->model->setMaximizeObjective(objective);
  // [END Student Assignment]
  return success();
}

// Define the createFormulation method for ALAPScheduling
mlir::LogicalResult
ALAPScheduling::createFormulation(mlir::ModuleOp moduleOp,
                                  const StaticTimingConfig &timingConfig) {
  // Create scheduling variables
  if (failed(createSchedulingVariables(moduleOp))) {
    return mlir::failure();
  }
  // Create dependency constraints
  if (failed(createDependencyConstraints(moduleOp, timingConfig))) {
    return mlir::failure();
  }
  // Add largest execution scheduling constraints
  if (failed(addALAPDeadlineConstraint(moduleOp, timingConfig))) {
    return mlir::failure();
  }
  // Create optimization objective
  if (failed(createOptimizationObjective(moduleOp))) {
    return mlir::failure();
  }

  return mlir::success();
}

// Function to return the largest execution clock cycle for a given basic block
// id
std::optional<unsigned> ALAPScheduling::getDeadline(unsigned bbID) {
  if (largestExecutionCycles.find(bbID) == largestExecutionCycles.end()) {
    return std::nullopt;
  }
  return largestExecutionCycles[bbID];
}

// Define the method to extract the largest execution clock cycles among all
// operations of each basic block from the ASAP result.
void ALAPScheduling::extractLargestExecutionCyclesPerBB(
    mlir::ModuleOp moduleOp,
    const llvm::MapVector<Operation *, unsigned> &asapResult,
    const StaticTimingConfig &timingConfig) {
  largestExecutionCycles.clear();
  // For each function in the module
  for (auto funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    // Use custom function of CFX dialect to get the operations separated by
    // basic blocks
    for (auto &block : funcOp.getCFXBasicBlocks()) {
      // For each basic block in the function find the latest finishing time
      // among all operations (start time + latency)
      unsigned maxFinishCycle = 0;
      for (auto *op : block) {
        unsigned startTime = asapResult.lookup(op);
        unsigned latency = timingConfig.getLatency(op).value_or(0);
        unsigned finishTime = startTime + latency;
        if (finishTime > maxFinishCycle) {
          maxFinishCycle = finishTime;
        }
      }
      // Save the deadline (latest finishing time) for the basic block
      largestExecutionCycles[block.getID()] = maxFinishCycle;
    }
  }
}

// Define the method to add constraints to ensure operations with largest
// execution cycles are scheduled at latest by their associated execution
// clock cycles
mlir::LogicalResult
ALAPScheduling::addALAPDeadlineConstraint(mlir::ModuleOp moduleOp,
                                           const StaticTimingConfig &timingConfig) {
  // [START Student Assignment]
  for (auto funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    for (auto &block : funcOp.getCFXBasicBlocks()) {
      std::optional<unsigned> deadline = this->getDeadline(block.getID());
      // If the BB does not have a deadline, we cannot perform ALAP scheduling.
      if (!deadline)
      // Inform the scheduler of the failure by returning an
      // mlir::LogicalResult back to the pass.
      return mlir::failure();
      // Convert from an optional<unsigned> to an unsigned.
      int latency = int(*deadline);
      for (auto *op : block) {
        CPVar sv_op = this->operationSchedulingVariables.lookup(op);
        int lat = int(timingConfig.getLatency(op).value_or(0.0));
        this->model->addConstr(sv_op + lat <= latency);
      }
    }
  }
  // [END Student Assignment]
  return mlir::success();
}

} // namespace dynamatic
