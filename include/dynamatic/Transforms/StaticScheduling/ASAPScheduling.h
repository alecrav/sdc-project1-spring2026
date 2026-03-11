//===- ASAPScheduling.h - ASAP scheduling pass --------*- C++ -*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the ASAP scheduling algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_TRANSFORMS_STATICSCHEDULING_ASAPSCHEDULING_H
#define DYNAMATIC_TRANSFORMS_STATICSCHEDULING_ASAPSCHEDULING_H

#include "dynamatic/Analysis/CFDFCAnalysis.h"
#include "dynamatic/Dialect/Handshake/HandshakeAttributes.h"
#include "dynamatic/Dialect/Handshake/HandshakeOps.h"
#include "dynamatic/Support/ConstraintProgramming/ConstraintProgramming.h"
#include "dynamatic/Support/LLVM.h"
#include "dynamatic/Support/MILP.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "dynamatic/Transforms/BufferPlacement/CFDFC.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"
#include "dynamatic/Transforms/StaticScheduling/StaticSchedulingILP.h"
#include "experimental/Support/StdProfiler.h"

namespace dynamatic {

// Class to represent ASAP scheduling algorithm that is derived from
// StaticSchedulingAlgorithm
class ASAPScheduling : public StaticSchedulingAlgorithm {
public:
  // Constructor. When useResourceConstraints is true, resourceConstraintsFile
  // must be a valid path to a JSON file containing resource constraints.
  ASAPScheduling(CPSolver::SolverKind solverKind, int timeout,
                 const std::string &milpName,
                 bool useResourceConstraints = false,
                 const std::string &resourceConstraintsFile = "")
      : StaticSchedulingAlgorithm(ASAP, solverKind, timeout, milpName),
        useResourceConstraints(useResourceConstraints),
        resourceConstraintsFile(resourceConstraintsFile) {};

  // Override the createFormulation method for ASAPScheduling
  mlir::LogicalResult
  createFormulation(mlir::ModuleOp moduleOp,
                    const StaticTimingConfig &timingConfig) override;
  // Override the createOptimizationObjective method for ASAPScheduling
  mlir::LogicalResult
  createOptimizationObjective(mlir::ModuleOp moduleOp) override;

  // [START LLVM RTTI prerequisites]
  static bool classof(const StaticSchedulingAlgorithm *b) {
    return b->getKind() == ASAP;
  }
  static bool classof(const ASAPScheduling *b) { return true; }
  // [END LLVM RTTI prerequisites]

private:
  bool useResourceConstraints;
  std::string resourceConstraintsFile;
};

} // namespace dynamatic
#endif // DYNAMATIC_TRANSFORMS_STATICSCHEDULING_ASAPSCHEDULING_H
