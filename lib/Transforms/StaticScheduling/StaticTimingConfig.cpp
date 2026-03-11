//===- StaticTimingConfig.cpp - Flat timing config for static scheduling -===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Transforms/StaticScheduling/StaticTimingConfig.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

using namespace dynamatic;
using namespace llvm;

std::optional<StaticTimingConfig>
StaticTimingConfig::readFromJSON(const std::string &path) {

  // Read the file into memory.
  ErrorOr<std::unique_ptr<MemoryBuffer>> fileOrErr =
      MemoryBuffer::getFile(path);
  if (!fileOrErr) {
    errs() << "StaticTimingConfig: failed to open '" << path
           << "': " << fileOrErr.getError().message() << "\n";
    return std::nullopt;
  }

  // Parse the buffer as JSON.
  Expected<json::Value> json =
      json::parse(fileOrErr.get()->getBuffer());
  if (!json) {
    errs() << "StaticTimingConfig: failed to parse JSON in '" << path
           << "': " << toString(json.takeError()) << "\n";
    return std::nullopt;
  }

  // The top-level value must be a JSON object (string keys -> numeric values).
  json::Object *obj = json->getAsObject();
  if (!obj) {
    errs() << "StaticTimingConfig: top-level value in '" << path
           << "' is not a JSON object\n";
    return std::nullopt;
  }

  // Validate that every value is a number and populate the StringMap.
  // Failing here rather than at query time means getLatency never needs
  // error handling.
  StaticTimingConfig config;
  for (const auto &entry : *obj) {
    StringRef key = entry.first;
    std::optional<double> val = entry.second.getAsNumber();
    if (!val) {
      errs() << "StaticTimingConfig: value for '" << key << "' in '"
             << path << "' is not a number\n";
      return std::nullopt;
    }
    config.latencies[key] = *val;
  }
  // All entries validated and loaded.
  return config;
}

std::optional<double>
StaticTimingConfig::getLatency(mlir::Operation *op) const {
  // The JSON keys are dialect-qualified operation names ("arith.addi"),
  // which is what getName().getStringRef() returns.
  auto it = latencies.find(op->getName().getStringRef());
  // Operation not in the timing configuration.
  if (it == latencies.end())
    return std::nullopt;
  // Return the latency.
  return it->second;
}
