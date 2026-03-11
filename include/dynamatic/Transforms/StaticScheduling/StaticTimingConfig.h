//===- StaticTimingConfig.h - Flat timing config for static scheduling -*- C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares StaticTimingConfig, a reader for the flat
// operation-name -> latency JSON used by static scheduling passes.
//
// TimingDatabase uses a nested bitwidth -> delay -> latency lookup designed
// for dynamic scheduling. Static scheduling only needs one latency per
// operation, so StaticTimingConfig reads a flat JSON and exposes a simpler
// interface.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_TRANSFORMS_STATICSCHEDULING_STATICTIMINGCONFIG_H
#define DYNAMATIC_TRANSFORMS_STATICSCHEDULING_STATICTIMINGCONFIG_H

#include "mlir/IR/Operation.h"
#include "llvm/ADT/StringMap.h"
#include <optional>
#include <string>

namespace dynamatic {

/// Operation latencies for static scheduling, read from a flat JSON file.
/// Replaces TimingDatabase for static scheduling passes, where each
/// operation has exactly one latency and the bitwidth/delay nesting
/// is not needed.
class StaticTimingConfig {
public:
  /// Loads the timing configuration from a path to a JSON file.
  /// The path should point to the flat JSON file
  /// (data-static/components-static.json).
  /// Returns std::nullopt and prints diagnostics on failure.
  static std::optional<StaticTimingConfig> readFromJSON(const std::string &path);

  /// Returns the latency for the given operation, or std::nullopt
  /// if the operation is not in the configuration.
  std::optional<double> getLatency(mlir::Operation *op) const;

private:
  /// Populated by readFromJSON.
  /// Using a StringMap rather than storing the json::Object directly
  /// so that all entries are validated as numbers at load time,
  /// not at each query.
  llvm::StringMap<double> latencies;
};

} // namespace dynamatic

#endif // DYNAMATIC_TRANSFORMS_STATICSCHEDULING_STATICTIMINGCONFIG_H
