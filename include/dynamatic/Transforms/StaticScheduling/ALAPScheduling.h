//===- ALAPScheduling.h - ALAP scheduling pass --------*- C++ -*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the ALAP scheduling algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_TRANSFORMS_STATICSCHEDULING_ALAPSCHEDULING_H
#define DYNAMATIC_TRANSFORMS_STATICSCHEDULING_ALAPSCHEDULING_H

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

// Class to represent ALAP scheduling algorithm that is derived from
// StaticSchedulingAlgorithm
class ALAPScheduling : public StaticSchedulingAlgorithm {
public:
  // Constructor
  ALAPScheduling(CPSolver::SolverKind solverKind, int timeout,
                 const std::string &milpName)
      : StaticSchedulingAlgorithm(ALAP, solverKind, timeout, milpName) {};
  // Override the createFormulation method for ALAPScheduling
  mlir::LogicalResult
  createFormulation(mlir::ModuleOp moduleOp,
                    const StaticTimingConfig &timingConfig) override;
  // Override the createOptimizationObjective method for ALAPScheduling
  mlir::LogicalResult
  createOptimizationObjective(mlir::ModuleOp moduleOp) override;

  // Function to extract the largest execution clock cycles among all
  // operations of each basic block from the ASAP result and save
  // it in the largestExecutionCycles member
  void extractLargestExecutionCyclesPerBB(
      mlir::ModuleOp moduleOp,
      const llvm::MapVector<Operation *, unsigned> &asapResult,
      const StaticTimingConfig &timingConfig);

  // Function to return the largest execution clock cycle for a given basic
  // block id
  std::optional<unsigned> getDeadline(unsigned bbID);

  // Function to add constraints to ensure each operation inside each basic
  // block has as maximum value of the scheduling variable the value saved in
  // largestExecutionCycles for that basic block
  mlir::LogicalResult addALAPDeadlineConstraint(mlir::ModuleOp moduleOp,
                                                 const StaticTimingConfig &timingConfig);

private:
  // Largest execution clock cycles among all operation in a basic block
  // The key is the id of the basic block and the value is the largest execution
  // clock cycle
  llvm::MapVector<unsigned, unsigned> largestExecutionCycles;

public:
  // [START LLVM RTTI prerequisites]
  static bool classof(const StaticSchedulingAlgorithm *b) {
    return b->getKind() == ALAP;
  }
  static bool classof(const ALAPScheduling *b) { return true; }
  // [END LLVM RTTI prerequisites]
};

} // namespace dynamatic
#endif // DYNAMATIC_TRANSFORMS_STATICSCHEDULING_ALAPSCHEDULING_H
