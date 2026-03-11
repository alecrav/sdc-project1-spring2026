#include "dynamatic/Transforms/StaticScheduling/StaticSchedulingILP.h"

using namespace dynamatic;

LogicalResult
StaticSchedulingAlgorithm::createSchedulingVariables(mlir::ModuleOp moduleOp) {
  // Check model is created
  if (!model) {
    llvm::errs() << "MILP model is not initialized.\n";
    return mlir::failure();
  }
  // Create scheduling variables for each operation in the module
  for (auto funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    // Use custom function of CFX dialect to get the operations separated by
    // basic blocks
    for (auto &block : funcOp.getCFXBasicBlocks()) {
      // Create scheduling variable for each operation
      for (auto *op : block) {
        // Get operation unique name
        std::string varName = "sv_" + getUniqueName(op).str();
        // Create an integer variable for the operation's scheduling time
        CPVar schedVar =
            model->addVar(varName, VarType::INTEGER, 0, std::nullopt);
        operationSchedulingVariables[op] = schedVar;
      }
    }
  }
  return mlir::success();
}

LogicalResult StaticSchedulingAlgorithm::createDependencyConstraints(
    mlir::ModuleOp moduleOp, const StaticTimingConfig &timingConfig) {
  // [START Student Assignment]
  // [END Student Assignment]
  return mlir::success();
}

// Function to create resource constraints for the MILP formulation
LogicalResult StaticSchedulingAlgorithm::createResourceConstraints(
    mlir::ModuleOp moduleOp, ResourceConstraints &resourceConstraints) {
  // [START Student Assignment]
  // [END Student Assignment]
  return mlir::success();
}
