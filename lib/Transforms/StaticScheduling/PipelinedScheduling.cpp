//===- PipelinedScheduling.cpp - Pipelined scheduling pass --------*- C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the Pipelined scheduling algorithm.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Transforms/StaticScheduling/PipelinedScheduling.h"
#include "dynamatic/Support/ConstraintProgramming/ConstraintProgramming.h"
#include "dynamatic/Support/LLVM.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"
#include "mlir/IR/Dominance.h"
#include "mlir/IR/Value.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

namespace dynamatic {

// Define the optimization function for PipelinedScheduling
mlir::LogicalResult
PipelinedScheduling::createOptimizationObjective(mlir::ModuleOp moduleOp) {
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
        objective -= schedVar;
      }
    }
  }

  this->model->setMaximizeObjective(objective);
  // [END Student Assignment]
  return success();
}

// Add pipeline constraints for a single basic block at a given II
mlir::LogicalResult PipelinedScheduling::createPipelineConstraints(
    const CFXBasicBlock &bb, unsigned ii,
    const StaticTimingConfig &timingConfig) {
  // [START Student Assignment]
  for (auto [branchOp, phiOp] : bb.getSelfLoopBackEdges()) {
    // CPVar
    CPVar sv_branch = this->operationSchedulingVariables.lookup(branchOp); 
    CPVar sv_phi = this->operationSchedulingVariables.lookup(phiOp);
    // latency
    int lat_branch = int(timingConfig.getLatency(branchOp).value_or(0.0));
    
    // add constraint
    this->model->addConstr(sv_branch + lat_branch <= sv_phi + ii);
  }
    
  for (auto [src, dst, dist] : bb.getMemoryDependencies()) {
    if (dist == 0) continue; 
    CPVar sv_dst = this->operationSchedulingVariables.lookup(dst);
    // predecessor operator scheduling variable
    int pred_lat = int(timingConfig.getLatency(src).value_or(0.0));
    CPVar sv_src = this->operationSchedulingVariables.lookup(src);
      
    // add constraint
    this->model->addConstr(sv_src + pred_lat <= sv_dst + ii);
  }
  
  // [END Student Assignment]
  return mlir::success();
}

// Build the complete MILP for one II attempt: base formulation plus pipeline
// constraints for all previously-solved BBs and the current BB.
mlir::LogicalResult PipelinedScheduling::createPipelinedFormulation(
    mlir::ModuleOp moduleOp, const StaticTimingConfig &timingConfig,
    const llvm::SmallVector<std::pair<CFXBasicBlock, unsigned>> &solvedBBs,
    const CFXBasicBlock &currentBB, unsigned ii) {
  // Build the base formulation: scheduling variables, dependency constraints,
  // and optimization objective
  if (failed(createFormulation(moduleOp, timingConfig)))
    return mlir::failure();
  // Re-add pipeline constraints for BBs already solved at their fixed IIs
  for (auto &[solvedBB, solvedII] : solvedBBs) {
    if (failed(createPipelineConstraints(solvedBB, solvedII, timingConfig)))
      return mlir::failure();
  }
  // Add pipeline constraints for the BB being solved at the trial II
  return createPipelineConstraints(currentBB, ii, timingConfig);
}

// Define the createFormulation method for PipelinedScheduling
mlir::LogicalResult
PipelinedScheduling::createFormulation(mlir::ModuleOp moduleOp,
                                       const StaticTimingConfig &timingConfig) {
  // Create scheduling variables
  if (failed(createSchedulingVariables(moduleOp))) {
    return mlir::failure();
  }
  // Create dependency constraints
  if (failed(createDependencyConstraints(moduleOp, timingConfig))) {
    return mlir::failure();
  }
  // Create optimization objective
  if (failed(createOptimizationObjective(moduleOp))) {
    return mlir::failure();
  }

  return mlir::success();
}

} // namespace dynamatic
