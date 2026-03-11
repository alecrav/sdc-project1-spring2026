//===- ResourceConstraints.h - Parse/Represent resource constraints -----*- C++
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

#ifndef DYNAMATIC_TRANSFORMS_STATICSCHEDULING_RESOURCECONSTRAINTS_H
#define DYNAMATIC_TRANSFORMS_STATICSCHEDULING_RESOURCECONSTRAINTS_H

#include "dynamatic/Support/LLVM.h"
#include "mlir/IR/Operation.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/JSON.h"

namespace dynamatic {

class ResourceType {
  std::string opType;
  unsigned availableResources;

public:
  ResourceType(StringRef opType, unsigned availableResources)
      : opType(opType.str()), availableResources(availableResources) {}
  bool needResource(mlir::Operation *op) const {
    return op->getName().getStringRef() == opType;
  }
  bool needResource(StringRef opType) const { return opType == this->opType; }
  unsigned getAvailableResources() const { return availableResources; }
  StringRef getOpType() const { return opType; }
};

class ResourceConstraints {
public:
  /// Type alias for const iterator over constraintResources
  using const_iterator = llvm::SmallVector<ResourceType>::const_iterator;

  /// Read resource constraints from a JSON file whose path is given as argument
  LogicalResult readFromJSON(std::string &jsonPath);

  /// Returns the maximum number of instances allowed for the operation type, if
  /// any exists. Returns std::nullopt if no constraint exists
  std::optional<unsigned> getMaxInstances(Operation *op);

  /// Returns the maximum number of instances allowed for the operation type, if
  /// any exists. Returns std::nullopt if no constraint exists
  std::optional<unsigned> getMaxInstances(StringRef opType);

  /// Returns the list of operation types that have resource constraints
  llvm::SmallVector<std::string> getConstrainedOperationTypes();

  const_iterator begin() const { return constraintResources.begin(); }
  const_iterator end() const { return constraintResources.end(); }

private:
  /// Maximum number of instances allowed for each operation type.
  llvm::SmallVector<ResourceType> constraintResources;
};
} // namespace dynamatic
#endif // DYNAMATIC_TRANSFORMS_STATICSCHEDULING_RESOURCECONSTRAINTS_H
