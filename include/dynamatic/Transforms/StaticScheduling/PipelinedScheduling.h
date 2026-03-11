//===- PipelinedScheduling.h -  Pipelined scheduling pass --------*- C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Pipelined scheduling algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_TRANSFORMS_STATICSCHEDULING_PIPELINEDSCHEDULING_H
#define DYNAMATIC_TRANSFORMS_STATICSCHEDULING_PIPELINEDSCHEDULING_H

#include "dynamatic/Analysis/CFDFCAnalysis.h"
#include "dynamatic/Dialect/Handshake/HandshakeAttributes.h"
#include "dynamatic/Dialect/Handshake/HandshakeOps.h"
#include "dynamatic/Support/LLVM.h"
#include "dynamatic/Support/MILP.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "dynamatic/Transforms/BufferPlacement/CFDFC.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"
#include "dynamatic/Transforms/StaticScheduling/StaticSchedulingILP.h"
#include "experimental/Support/StdProfiler.h"

namespace dynamatic {

// Class to represent Pipelined scheduling algorithm that is derived from
// StaticSchedulingAlgorithm
class PipelinedScheduling : public StaticSchedulingAlgorithm {
public:
  // Constructor
  PipelinedScheduling(CPSolver::SolverKind solverKind, int timeout,
                      const std::string &milpName)
      : StaticSchedulingAlgorithm(PIPELINED, solverKind, timeout, milpName) {};
  // Override the createFormulation method for PipelinedScheduling
  mlir::LogicalResult
  createFormulation(mlir::ModuleOp moduleOp,
                    const StaticTimingConfig &timingConfig) override;
  // Override the createOptimizationObjective method for PipelinedScheduling
  mlir::LogicalResult
  createOptimizationObjective(mlir::ModuleOp moduleOp) override;

  // Build the complete MILP for one II attempt: base formulation, pipeline
  // constraints for all previously-solved BBs at their fixed IIs, and pipeline
  // constraints for the current BB at the trial II.
  mlir::LogicalResult createPipelinedFormulation(
      mlir::ModuleOp moduleOp, const StaticTimingConfig &timingConfig,
      const llvm::SmallVector<std::pair<CFXBasicBlock, unsigned>> &solvedBBs,
      const CFXBasicBlock &currentBB, unsigned ii);

  // Add pipeline constraints for a single basic block at a given II.
  mlir::LogicalResult createPipelineConstraints(
      const CFXBasicBlock &bb, unsigned ii,
      const StaticTimingConfig &timingConfig);

  // [START LLVM RTTI prerequisites]
  static bool classof(const StaticSchedulingAlgorithm *b) {
    return b->getKind() == PIPELINED;
  }
  static bool classof(const PipelinedScheduling *b) { return true; }
  // [END LLVM RTTI prerequisites]
};

} // namespace dynamatic
#endif // DYNAMATIC_TRANSFORMS_STATICSCHEDULING_PIPELINEDSCHEDULING_H
