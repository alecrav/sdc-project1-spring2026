//===- StaticScheduling.cpp - Static scheduling of CDFG ---------*- C++ -*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Static scheduling of CDFG, using a set of available algorithms. Execution
// states are decided for each operation in the CDFG.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Support/Attribute.h"
#include "dynamatic/Support/CFG.h"
#include "dynamatic/Support/ConstraintProgramming/ConstraintProgramming.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "dynamatic/Transforms/StaticScheduling/PipelinedScheduling.h"
#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"
#include "dynamatic/Transforms/StaticScheduling/SchedulingIO.h"
#include "dynamatic/Transforms/StaticScheduling/StaticSchedulingILP.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlowOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Dominance.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/OperationSupport.h"
#include "mlir/Support/IndentedOstream.h"
#include "mlir/Support/LogicalResult.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <filesystem>
#include <string>
#include <system_error>

using namespace mlir;
using namespace dynamatic;

// List of algorithms supported for static scheduling
static constexpr llvm::StringLiteral ASAP("asap");
static constexpr llvm::StringLiteral ALAP("alap");
static constexpr llvm::StringLiteral PIPELINED("pipelined");
static constexpr llvm::StringLiteral
    ASAP_RESOURCE_CONSTRAINED("asap_resource_constrained");

// The following inclusion is necessary to make sure that the passes are
// included correctly.
#include "dynamatic/Transforms/Passes.h"
namespace dynamatic {
#define GEN_PASS_DEF_STATICSCHEDULING
#include "dynamatic/Transforms/Passes.h.inc"
} // namespace dynamatic

namespace dynamatic {
struct StaticSchedulingPass
    : public dynamatic::impl::StaticSchedulingBase<StaticSchedulingPass> {

  /// Trivial field-by-field constructor.
  StaticSchedulingPass(StringRef algorithm, StringRef timingModels,
                       unsigned timeout, bool dumpLogs);

  /// Use the auto-generated construtors from tblgen
  using StaticSchedulingBase::StaticSchedulingBase;

  // Log results
  void logResults(mlir::raw_indented_ostream &os,
                  llvm::MapVector<Operation *, unsigned> &result);

  // Function to create formulation, solve it, and extract results
  mlir::LogicalResult createAndSolveFormulation(
      StaticSchedulingAlgorithm &schedulingAlgorithm, mlir::ModuleOp moduleOp,
      const StaticTimingConfig &timingConfig,
      llvm::MapVector<Operation *, unsigned> &result,
      mlir::raw_indented_ostream &os, int *milpStatus = nullptr);

  void runOnOperation() override;
};
} // namespace dynamatic

// Define the method to create, solve the formulation, and extract results
mlir::LogicalResult StaticSchedulingPass::createAndSolveFormulation(
    StaticSchedulingAlgorithm &schedulingAlgorithm, mlir::ModuleOp moduleOp,
    const StaticTimingConfig &timingConfig,
    llvm::MapVector<Operation *, unsigned> &result,
    mlir::raw_indented_ostream &os, int *milpStatus) {
  // Create MILP formulation
  if (failed(schedulingAlgorithm.createFormulation(moduleOp, timingConfig))) {
    llvm::errs() << "=== Static scheduling MILP formulation failed ===\n";
    return mlir::failure();
  }
  // Solve MILP formulation
  if (failed(schedulingAlgorithm.solve(milpStatus))) {
    llvm::errs() << "=== Static scheduling MILP solving failed ===\n";
    return mlir::failure();
  }
  // Extract scheduling result from MILP solution
  schedulingAlgorithm.extractResult(result);
  // Check result is not empty
  if (result.empty()) {
    llvm::errs() << "=== Static scheduling result extraction failed ===\n";
    return mlir::failure();
  }
  // Write result in logger
  logResults(os, result);
  return mlir::success();
}

// Define the method to log scheduling results
void StaticSchedulingPass::logResults(
    mlir::raw_indented_ostream &os,
    llvm::MapVector<Operation *, unsigned> &result) {
  os << "\n# ====================== #\n";
  os << "# Static Scheduling Result #\n";
  os << "# ====================== #\n\n";
  for (auto [op, schedTime] : result) {
    os << "Operation: " << getUniqueName(op)
       << " scheduled at time: " << schedTime << "\n";
  }
  os.flush();
}


void StaticSchedulingPass::runOnOperation() {

  ModuleOp modOp = llvm::dyn_cast<ModuleOp>(getOperation());
  NameAnalysis &nameAnalysis = getAnalysis<NameAnalysis>();
  if (!nameAnalysis.isAnalysisValid())
    return signalPassFailure();

  // Read timing config from JSON file
  auto timingConfigOpt = StaticTimingConfig::readFromJSON(timingModels);
  if (!timingConfigOpt) {
    llvm::errs() << "=== StaticTimingConfig read failed ===\n";
    return signalPassFailure();
  }
  const StaticTimingConfig &timingConfig = *timingConfigOpt;

  CPSolver::SolverKind solverKind;
  if (solver == "gurobi") {
#ifdef DYNAMATIC_GUROBI_NOT_INSTALLED
    llvm::report_fatal_error("Gurobi not installed!");
#else
    solverKind = CPSolver::GUROBI;
#endif // DYNAMATIC_GUROBI_NOT_INSTALLED
  } else if (solver == "cbc") {
#ifdef DYNAMATIC_ENABLE_CBC
    solverKind = CPSolver::CBC;
#else
    llvm::report_fatal_error("CBC not installed!");
#endif // DYNAMATIC_ENABLE_CBC
  } else {
    llvm::errs() << "Solver type: " << solver << " is not supported!\n";
    llvm::report_fatal_error("Unsupported solver type!");
  }
  unsigned timeout = this->timeout;
  std::string logFolder = "static-scheduling";
  std::string milpName = logFolder + llvm::sys::path::get_separator().str() +
                         "static-scheduling-" + algorithm;

  if (dumpLogs) {
    std::filesystem::create_directories(logFolder);
  }

  llvm::MapVector<llvm::StringRef,
                  std::function<std::unique_ptr<StaticSchedulingAlgorithm>()>>
      schedAlgos;
  // Fill in initial mapping of algorithms
  schedAlgos[ASAP] =
      [solverKind, timeout,
       milpName]() -> std::unique_ptr<StaticSchedulingAlgorithm> {
    return std::make_unique<ASAPScheduling>(solverKind, timeout, milpName);
  };
  schedAlgos[ALAP] =
      [solverKind, timeout,
       milpName]() -> std::unique_ptr<StaticSchedulingAlgorithm> {
    return std::make_unique<ALAPScheduling>(solverKind, timeout, milpName);
  };
  schedAlgos[PIPELINED] =
      [solverKind, timeout,
       milpName]() -> std::unique_ptr<StaticSchedulingAlgorithm> {
    return std::make_unique<PipelinedScheduling>(solverKind, timeout, milpName);
  };
  std::string resourceConstraintsFile = this->resourceConstraints;
  schedAlgos[ASAP_RESOURCE_CONSTRAINED] =
      [solverKind, timeout, milpName,
       resourceConstraintsFile]() -> std::unique_ptr<StaticSchedulingAlgorithm> {
    return std::make_unique<ASAPScheduling>(solverKind, timeout, milpName,
                                            /*useResourceConstraints=*/true,
                                            resourceConstraintsFile);
  };

  // Check that the algorithm is supported
  if (!schedAlgos.contains(algorithm)) {
    llvm::errs() << "Unknown algorithm '" << algorithm
                 << "', possible choices are:\n";
    for (auto &algo : schedAlgos)
      llvm::errs() << "\t- " << algo.first << "\n";
    return signalPassFailure();
  }
  std::unique_ptr<StaticSchedulingAlgorithm> schedulingAlgorithm =
      schedAlgos[algorithm]();

  // Create logging infrastructure
  std::string sep = llvm::sys::path::get_separator().str();
  std::error_code ec;

  // Make sure that the logger is properly deallocated at the end
  auto fs = llvm::raw_fd_stream(logFolder + sep + "static-scheduling.log", ec);

  if (static_cast<bool>(ec)) {
    llvm::errs() << "Failed to create the log file!\n";
    return signalPassFailure();
  }

  auto os = mlir::raw_indented_ostream(fs);

  if (algorithm == ALAP) {
    // For ALAP scheduling, we need to solve first ASAP to get the
    // maximum scheduling times of sink operations
    ASAPScheduling asapScheduling(solverKind, timeout, milpName + "-asap");
    llvm::MapVector<Operation *, unsigned> asapResult;
    if (failed(createAndSolveFormulation(asapScheduling, modOp, timingConfig,
                                         asapResult, os))) {
      llvm::errs() << "=== ASAP scheduling MILP creation/solving failed ===\n";
      return signalPassFailure();
    }
    // Check the result is not empty
    if (asapResult.empty()) {
      llvm::errs() << "=== ASAP scheduling result extraction failed ===\n";
      return signalPassFailure();
    }

    if (!llvm::isa<ALAPScheduling>(schedulingAlgorithm.get())) {
      llvm::errs() << "schedulingAlgorithm has incorrect type\n";
      return signalPassFailure();
    }

    auto *alapScheduling =
        llvm::cast<ALAPScheduling>(schedulingAlgorithm.get());

    alapScheduling->extractLargestExecutionCyclesPerBB(modOp, asapResult,
                                                       timingConfig);
  }

  llvm::MapVector<Operation *, unsigned> result;

  if (algorithm == PIPELINED) {
    unsigned maximumII = this->maximumII;
    if (maximumII < 1) {
      llvm::errs() << "Maximum initiation interval must be at least 1 "
                      "for pipelined scheduling. Current value is "
                   << maximumII << ".\n";
      return signalPassFailure();
    }

    cfx::FuncOp funcOp = *modOp.getOps<cfx::FuncOp>().begin();
    llvm::SmallVector<CFXBasicBlock> pipelinableBBs;
    for (auto &bb : funcOp.getCFXBasicBlocks()) {
      if (bb.isPipelinable())
        pipelinableBBs.push_back(bb);
    }

    llvm::errs() << "=== Pipelined scheduling: " << pipelinableBBs.size()
                 << " pipelinable BB(s), maximum II " << maximumII << " ===\n";

    if (pipelinableBBs.empty()) {
      // No loops to pipeline — solve with base constraints only
      if (failed(createAndSolveFormulation(*schedulingAlgorithm, modOp,
                                            timingConfig, result, os))) {
        llvm::errs() << "=== Pipelined scheduling MILP failed (no pipelinable "
                        "BBs) ===\n";
        return signalPassFailure();
      }
    } else {
      // Solve each pipelinable BB separately, finding its minimum feasible II
      llvm::SmallVector<std::pair<CFXBasicBlock, unsigned>> solvedBBs;

      for (auto &currentBB : pipelinableBBs) {
        bool solved = false;
        for (unsigned ii = 1; ii <= maximumII; ++ii) {
          llvm::errs() << "=== Trying BB " << currentBB.getID()
                       << " with II " << ii << " ===\n";
          // Each II attempt needs a fresh MILP instance
          schedulingAlgorithm = schedAlgos[algorithm]();
          auto *pipelinedScheduling =
              llvm::cast<PipelinedScheduling>(schedulingAlgorithm.get());

          if (failed(pipelinedScheduling->createPipelinedFormulation(
                  modOp, timingConfig, solvedBBs, currentBB, ii))) {
            llvm::errs() << "=== MILP formulation failed for BB "
                         << currentBB.getID() << " with II " << ii << " ===\n";
            return signalPassFailure();
          }

          int milpStatus = 0;
          if (failed(pipelinedScheduling->solve(&milpStatus))) {
            if (milpStatus != CPSolver::INFEASIBLE) {
              llvm::errs() << "=== MILP solving failed for BB "
                           << currentBB.getID() << " with II " << ii
                           << " (status " << milpStatus << ") ===\n";
              return signalPassFailure();
            }
            continue;
          }

          solvedBBs.push_back({currentBB, ii});
          llvm::errs() << "BB " << currentBB.getID()
                       << " feasible at II " << ii << "\n";
          llvm::errs() << "=== Locked in BB " << currentBB.getID()
                       << " with II " << ii << " ===\n";
          solved = true;
          break;
        }
        if (!solved) {
          llvm::errs() << "=== BB " << currentBB.getID()
                       << " infeasible for all IIs up to " << maximumII
                       << " ===\n";
          return signalPassFailure();
        }
      }

      // Extract result from the final solve (includes all BBs' constraints)
      schedulingAlgorithm->extractResult(result);
      logResults(os, result);

      // Persist each BB's II to the IR for the visualizer
      for (auto &[bb, ii] : solvedBBs) {
        funcOp.setInitiationIntervalForBlock(bb.getID(), ii);
      }
    }
  }

  // Create, solve formulation, and extract results
  if (result.empty() &&
      failed(createAndSolveFormulation(*schedulingAlgorithm, modOp, timingConfig,
                                       result, os))) {
    llvm::errs() << "=== Static scheduling MILP creation/solving failed for "
                 << algorithm << " scheduling ===\n";
    return signalPassFailure();
  }

  // Add for each operation its scheduling time as an attribute
  // Iterate through each function
  for (auto funcOp : modOp.getOps<cfx::FuncOp>()) {
    if (failed(funcOp.addSchedulingAttributes(result))) {
      llvm::errs() << "=== Static scheduling attribute addition failed ===\n";
      return signalPassFailure();
    }
  }

  // Print scheduled graph
  printSchedulingResult(os, result, modOp, timingConfig);
  // Write scheduled graph in dot format
  std::string dotFilename =
      logFolder + sep + "static-scheduling-" + this->algorithm + ".dot";
  writeScheduledDOTPerBlock(result, modOp, dotFilename, timingConfig);
  // Translate graph in dot format into pdf
  std::string pdfFilename =
      logFolder + sep + "static-scheduling-" + this->algorithm + ".pdf";
  std::string dotCommand = "dot -Tpdf " + dotFilename + " -o " + pdfFilename;
  int dotRet = std::system(dotCommand.c_str());
  if (dotRet != 0) {
    llvm::errs() << "Warning: Failed to generate PDF from DOT file using "
                 << "command: " << dotCommand << "\n";
  }
}
