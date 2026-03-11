
//===- SchedulingIO.h - Support for input/output files managing  -----------*-
// C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares support for input/output files managing for static
// scheduling
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Dialect/Handshake/HandshakeOps.h"
#include "dynamatic/Support/DynamaticPass.h"
#include "dynamatic/Support/LLVM.h"
#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "dynamatic/Transforms/StaticScheduling/ALAPScheduling.h"
#include "dynamatic/Transforms/StaticScheduling/ASAPScheduling.h"
#include "dynamatic/Transforms/StaticScheduling/PipelinedScheduling.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/IndentedOstream.h"
#include "mlir/Support/LogicalResult.h"
#include "llvm/ADT/MapVector.h"
#include <string>

namespace dynamatic {
// Function to print the scheduling results in a graphical format
void printSchedulingResult(mlir::raw_indented_ostream &os,
                           llvm::MapVector<Operation *, unsigned> &schedule,
                           ModuleOp moduleOp, const StaticTimingConfig &timingConfig);

// Function to write scheduled graph in dot format per basic block
void writeScheduledDOTPerBlock(const llvm::MapVector<Operation *, unsigned> &,
                               ModuleOp moduleOp, std::string &filename,
                               const StaticTimingConfig &timingConfig);

} // namespace dynamatic
