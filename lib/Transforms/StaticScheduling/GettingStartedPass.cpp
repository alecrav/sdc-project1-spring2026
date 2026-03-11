//===- GettingStartedPass.cpp - Getting started pass -------------*- C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The getting-started pass for the SDC course. Students implement debug
// printing functions here: hello world, operation printing, and dependency
// printing.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Support/CFG.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "mlir/IR/BuiltinOps.h"
#include "llvm/Support/raw_ostream.h"

#include "dynamatic/Transforms/Passes.h"
namespace dynamatic {
#define GEN_PASS_DEF_GETTINGSTARTED
#include "dynamatic/Transforms/Passes.h.inc"
} // namespace dynamatic

using namespace mlir;
using namespace dynamatic;

static void helloWorld() {
  // [START Student Assignment]
  // [END Student Assignment]
}

// Loop through all operations in the module and print out their MLIR syntax
// and latency, organized by basic block.
static void printOperations(mlir::ModuleOp moduleOp,
                            const StaticTimingConfig &timingConfig) {
  // [START Student Assignment]
  // [END Student Assignment]
}

// Loop through all operations and print out the intra-BB data and memory
// dependencies (excluding loop-carried dependencies).
static void printDataAndMemoryDependencies(mlir::ModuleOp moduleOp) {
  // [START Student Assignment]
  // [END Student Assignment]
}

// Print out all loop-carried data and memory dependencies.
static void printLoopCarriedDependencies(mlir::ModuleOp moduleOp) {
  // [START Student Assignment]
  // [END Student Assignment]
}

namespace dynamatic {
struct GettingStartedPass
    : public dynamatic::impl::GettingStartedBase<GettingStartedPass> {

  using GettingStartedBase::GettingStartedBase;

  void runOnOperation() override;
};
} // namespace dynamatic

void GettingStartedPass::runOnOperation() {
  ModuleOp modOp = llvm::dyn_cast<ModuleOp>(getOperation());

  // Read timing config from JSON file
  auto timingConfigOpt = StaticTimingConfig::readFromJSON(timingModels);
  if (!timingConfigOpt) {
    llvm::errs() << "=== StaticTimingConfig read failed ===\n";
    return signalPassFailure();
  }
  const StaticTimingConfig &timingConfig = *timingConfigOpt;

  helloWorld();
  printOperations(modOp, timingConfig);
  printDataAndMemoryDependencies(modOp);
  printLoopCarriedDependencies(modOp);
}
