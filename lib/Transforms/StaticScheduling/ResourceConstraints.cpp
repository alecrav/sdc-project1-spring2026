//===- ResourceConstraints.cpp - Parse/Represent resource constraints -----*-
// C++
//-*-===//
//
// Dynamatic is under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the infrastrucure to parse, represent, and query resource
// constraints for MLIR operations during static scheduling. The resource
// constraints must be represented in a JSON data format which can then be
// deserialized into the C++ data types defined in this file.
//
//===----------------------------------------------------------------------===//

#include "dynamatic/Transforms/StaticScheduling/ResourceConstraints.h"
#include "mlir/IR/Operation.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>

namespace dynamatic {

// Function to read resource constraints from a JSON file whose path is given as
// argument
LogicalResult ResourceConstraints::readFromJSON(std::string &jsonPath) {
  // Open the JSON file
  std::ifstream jsonFile(jsonPath);
  if (!jsonFile.is_open()) {
    llvm::errs() << "Failed to open resource constraints file: " << jsonPath
                 << "\n";
    return failure();
  }

  // Read the JSON content from the file and into a string
  std::string jsonString;
  std::string line;
  while (std::getline(jsonFile, line))
    jsonString += line;

  // Try to parse the string as a JSON
  llvm::Expected<llvm::json::Value> value = llvm::json::parse(jsonString);
  if (!value) {
    llvm::errs() << "Failed to parse resource constraints in \"" << jsonPath
                 << "\"\n";
    return failure();
  }

  const llvm::json::Object *resourceConstraintsObj = value->getAsObject();
  if (!resourceConstraintsObj) {
    llvm::errs() << "Resource constraints JSON is not an object\n";
    return failure();
  }

  // Parse max instances for each operation type
  for (const auto &member : *resourceConstraintsObj) {
    StringRef opType = member.first;
    std::optional<uint64_t> maxInst = member.second.getAsUINT64();
    if (!maxInst) {
      llvm::errs() << "Max instances for operation type \"" << opType
                   << "\" is not an unsigned integer\n";
      return failure();
    }
    assert(maxInst >= 0 && "Max instances must be non-negative");
    ResourceType resourceType(opType, static_cast<unsigned>(*maxInst));
    constraintResources.push_back(resourceType);
  }

  return success();
}

// Function to get the maximum number of instances allowed for the operation
// type, if any exists
std::optional<unsigned> ResourceConstraints::getMaxInstances(Operation *op) {
  for (const auto &resourceType : constraintResources) {
    if (resourceType.needResource(op)) {
      return resourceType.getAvailableResources();
    }
  }
  return std::nullopt;
}

// Function to get the maximum number of instances allowed for the given
// operation type, if any exists
std::optional<unsigned> ResourceConstraints::getMaxInstances(StringRef opType) {
  for (const auto &resourceType : constraintResources) {
    if (resourceType.needResource(opType)) {
      return resourceType.getAvailableResources();
    }
  }
  return std::nullopt;
}

// Function to get the list of operation types that have resource constraints
llvm::SmallVector<std::string>
ResourceConstraints::getConstrainedOperationTypes() {
  llvm::SmallVector<std::string> opTypes;
  for (const auto &resourceType : constraintResources) {
    if (resourceType.getAvailableResources() > 0) {
      opTypes.push_back(resourceType.getOpType().str());
    }
  }
  return opTypes;
}

} // namespace dynamatic
