# Static Scheduling

This document describes the infrastructure that executes static scheduling in Dynamatic.

## Overview

This section describes the `static-scheduling` infrastructure in Dynamatic. This pass receives as input an IR written in the CFX dialect and returns the same IR with each operation annotated with a new attribute called `sv` (scheduling variable). The value of this attribute corresponds to the clock cycle in which each operation executes.

**IMPORTANT**: The input CFX is assumed to contain a unique function with a unique block containing the flattened CDFG. This block can contain multiple basic blocks and for each operation the basic block information is expressed as an attribute ("cfx.bb"). This attribute of an operation can be retrieved using the following function call `funcOp.getBB(op)` where `funcOp` is the cfx::funcOp operation representing the top function and `op` is the operation itself; this function returns the id of the BB. Additionally, there is the custom CFX function `funcOp.getCFXBasicBlocks()` which returns the list of basic blocks objects, `CFXBasicBlock`. Each object contains the list of operations in the BB and the corresponding id. The function `getTopologicallySortedOps` of this class returns the list of operations in topological order.

## Arguments

This pass receives as input the following arguments:
- `algorithm`: it represents the scheduling algorithm to use,
- `timing-models`: it represents the path to the JSON-formatted file containing timing models for each operation,
- `resource-constraints`: it represents the path to the JSON-formatted file containing the resource constraints,
- `target-period`: it represents the target clock period (in ns),
- `maximum-ii`: it represents the maximum value of initiation interval for pipelined scheduling,
- `timeout`: it represents the timeout for the static scheduling MILP (in s),
- `dump-logs`: if true, dump debugging information.

## Implementation Structure

The file that manages the pass is [StaticScheduling.cpp](lib/Transforms/StaticScheduling/StaticScheduling.cpp). In this file, there are four main steps:
1. MILP solver selection which depends on the available solvers (Gurobi or CBC),
2. Static Scheduling algorithm selection,
3. Timing Database extraction (for more info please refer to [timing doc](docs/DeveloperGuide/Specs/TimingCharacterization.md)),
4. MILP creation and solution.

The MILP creation step is based on a common class `StaticSchedulingAlgorithm` which derives from the `MILP` class. In the `StaticSchedulingAlgorithm` class, there are the following common classes among all static scheduling algorithms:
1. `createFormulation` which creates the MILP formulation,
2. `solve` which solves the MILP formulation,
3. `createOptimizationObjective` which creates the optimization objective function,
4. `extractResult` which extracts the results from the MILP solution,
5. `createSchedulingVariables` which creates the scheduling variables,
6. `createDependencyConstraints` which creates the dependency constraints.

Only functions 1 and 3 are specific to each static scheduling algorithm. The other functions are common to all of them. 

### Key Functions
We detail the behaviour of two key functions: `createSchedulingVariables` and `createDependencyConstraints`.

The former creates for each operation a scheduling variable in the MILP formulation and saves them in the map structure `operationSchedulingVariables`.


The latter function creates for each pair of operation nodes a dependency constraint to ensure that for each edge in the graph the destination executes after the source has executed using the following equation:
```
sv(dst) >= sv(src) + lat(src)
```
where `sv` is the scheduling variable, `dst` is the destination of the edge, `src` is the source of the edge, and `lat` is the latency of the operation. The latency value is obtained from the timing database. An important assumption of this function is the independence of each basic block from another basic block. 


## Static Scheduling Algorithms

In the rest of this doc, we detail the specific features of each static scheduling algorithm.

### ASAP Scheduling

The `createFormulation` function of this algorithm executes the following steps:
1. Create scheduling variables using `createSchedulingVariables` function.
2. Create dependency constraints `createDependencyConstraints` function.
3. Create the optimization function using `createOptimizationObjective` function.

The optimization function of this algorithm is the following: min($\sum_{n}^{N} sv(n)$) where $N$ is the set of all operations in the input MLIR graph.

### ALAP Scheduling

It has the same structure as the ASAP scheduling but with a different optimization function: max($\sum_{n}^{N} sv(n)$) where $N$ is the set of all operations in the input MLIR graph. 

In addition to the objective function, the algorithm introduces the following constraint:
```
sv(op) <= largestExecutionCycles[bb]
```
which enforces an upper bound on the scheduling variable of operation op which resides in basic block bb. This is applied to every operation inside each basic block.

The constraint is applied to every operation inside each basic block and it gets assigned the value of the maximum scheduling variable value within their respective basic block obtained from a previous ASAP scheduling run. The corresponding scheduling variable values are extracted from that prior run.

In order to add this constraint, the algorithm contains two extra functions:

1. `extractLargestExecutionCyclesPerBB` extracts the largest scheduling variables values per BB computed by a previously executed scheduling algorithm and stores them in the `largestExecutionCycles` map.

2. `addLargestExecutionSchedulingConstraints` adds scheduling constraints that enforce an upper bound on the scheduling variables of the operations inside each basic block.

### Pipelined Scheduling

It has the same structure as the ASAP scheduling but it adds an extra constraint on the back edges as follows:
```
sv(dst) >= sv(src) + latency(src) - II
```

where II is the initiation interval. This constraint allows to check if it is possible to pipeline with the initiation interval II.

In order to add this constraint, the following extra function is added:

- `createPipelineConstraints` creates the pipeline constraint for each loop back edge obtained from `getSelfLoopBackEdgesInCDFG` function from the CFX dialect.

Since certain values of II cannot generate a working pipelined circuit, the code iterates through different values of II starting from the smallest possible value (1) until reaching the maximum II value which is user-defined through the pass argument `maximum-ii`.

## Resource Constraints

Each scheduling algorithm can have additional resource constraints for each type of operation. These resource constraints are specified in the input file specified by the pass argument `resource-constraints`. The input file should be a JSON with the following format:
```json
{
  "op-type1": maxInstances1,
  "op-type2": maxInstances2
}
```
where `op-type` is the type of an operation and `maxInstances` is the unsigned representing the maximum number of instances of the operation type. An example of JSON file is the following:
```json
{
  "arith.addi": 2,
  "arith.subi": 2
}
```
which specifies that integer additions and subtractrions can have maximim two instances at each clock cycle.

The resource constraints are managed using the utility class `ResourceConstraints` defined in the  [ResourceConstraints.cpp](lib/Transforms/StaticScheduling/ResourceConstraints.cpp) file. This class defines the following functions:

1. `readFromJSON` reads the resource constraints from the input JSON file specified by input file `resource-constraints` and saves it in the internal data structure `maxInstances`,

2. `getMaxInstances` returns the maximum number of instances for an operation or an operation type,

3. `getConstrainedOperationTypes` returns the operation types that have a resource constraint associated.

The resource constraints are added to the MILP with the function `createResourceConstraints` which adds the following constraint to the MILP:
```
sv(opA) >= sv(opB) + 1
```
which ensures that operation `opA` starts execution after operation `opB`.

The function uses the topological order to determine which operation should execute first. The topological order is computed with the function `getTopologicallySortedOps` of the `CFXBasicBlock` class.

## Extra Functions

There are the following extra functions described in the [CDFGSupport.cpp](lib/Transforms/StaticScheduling/CDFGSupport.cpp) file:

1. `setInitiationIntervalForBlock` sets the initiation interval of a basic block by setting the `II` attribute of the front operation of the basic block,

2. `getInitiationIntervalForBlock` retrieves the initial interval of a basic block extracting the `II` attribute.

There are the following extra functions described in the [SchedulingIO.cpp](lib/Transforms/StaticScheduling/SchedulingIO.cpp) file:

1. `printSchedulingResult` prints the scheduling results on terminal for each operation,

2. `writeScheduledDOTPerBlock` exports the CDFG with annotated scheduling results for each operation and for each BB separately.