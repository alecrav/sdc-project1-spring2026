#include "dynamatic/Transforms/StaticScheduling/StaticSchedulingILP.h"

using namespace dynamatic;

LogicalResult StaticSchedulingAlgorithm::createSchedulingVariables(mlir::ModuleOp moduleOp) {
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
    mlir::ModuleOp moduleOp, const StaticTimingConfig &timingConfig
  ) {
  // [START Student Assignment]
  for (cfx::FuncOp funcOp : moduleOp.getOps<cfx::FuncOp>()) {
    for (const CFXBasicBlock& bb : funcOp.getCFXBasicBlocks()) {
      llvm::errs() << "=== Analyzing BB #" << bb.getID() << " ===\n";
      for (Operation *op : bb) {
        for (Operation *op : bb) {
          // iterate through predecessors
          for (mlir::Value operandValue : op->getOperands()) {
            // If the input value to an operation is a function argument,
            // there is no explicit predecessor.
            if (llvm::isa<BlockArgument>(operandValue) || llvm::isa<cfx::PhiOp>(op)) continue;
            if (bb.hasOp(op)) {
              // current operation scheduling variable
              CPVar sv_op = this->operationSchedulingVariables.lookup(op);
              // predecessor operator scheduling variable
              Operation *predecessor = operandValue.getDefiningOp();
              int pred_lat = int(timingConfig.getLatency(predecessor).value_or(0.0));
              CPVar sv_pred = this->operationSchedulingVariables.lookup(predecessor);

              // add constraint
              this->model->addConstr(sv_pred + pred_lat <= sv_op);
            }
          }
        }
        for (auto [src, dst, dist] : bb.getMemoryDependencies()) {
          if (dist == 0) {
            CPVar sv_dst = this->operationSchedulingVariables.lookup(dst);
            // predecessor operator scheduling variable
            int pred_lat = int(timingConfig.getLatency(src).value_or(0.0));
            CPVar sv_src = this->operationSchedulingVariables.lookup(src);
            
            // add constraint
            this->model->addConstr(sv_src + pred_lat <= sv_dst);
          }
        }
      }
    }
  }
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
