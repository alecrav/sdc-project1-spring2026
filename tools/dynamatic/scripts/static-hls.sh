#!/bin/bash

source "$1"/tools/dynamatic/scripts/utils.sh

# ============================================================================ #
# Variable definitions
# ============================================================================ #

# Script arguments
DYNAMATIC_DIR=$1
SRC_DIR=$2
OUTPUT_DIR=$3
KERNEL_NAME=$4
GETTING_STARTED=${5}
SCHEDULING_ALGORITHM=${6}
MILP_SOLVER=${7}
MAX_II=${8}
RESOURCE_CONSTRAINTS=${9}

LLVM=$DYNAMATIC_DIR/llvm-project
LLVM_BINS=$DYNAMATIC_DIR/bin
export PATH=$PATH:$LLVM_BINS

CLANGXX_BIN="$DYNAMATIC_DIR/bin/clang++"
LLVM_OPT="$LLVM_BINS/opt"
LLVM_TO_STD_TRANSLATION_BIN="$DYNAMATIC_DIR/build/bin/translate-llvm-to-std"
DYNAMATIC_OPT_BIN="$DYNAMATIC_DIR/bin/dynamatic-opt"
DYNAMATIC_PROFILER_BIN="$DYNAMATIC_DIR/bin/exp-frequency-profiler"
DYNAMATIC_EXPORT_DOT_BIN="$DYNAMATIC_DIR/bin/export-dot"
DYNAMATIC_EXPORT_CFG_BIN="$DYNAMATIC_DIR/bin/export-cfg"

# Generated directories/files
COMP_DIR="$OUTPUT_DIR/comp"

F_C_SOURCE="$SRC_DIR/$KERNEL_NAME.c" 

F_CLANG="$COMP_DIR/clang.ll"
F_CLANG_OPTIMIZED="$COMP_DIR/clang.opt.ll"
F_CLANG_OPTIMIZED_DEPENDENCY="$COMP_DIR/clang.opt.dep.ll"

F_CF="$COMP_DIR/cf.mlir"
F_CF_TRANSFORMED="$COMP_DIR/cf_transformed.mlir"
F_CF_DYN_TRANSFORMED_MEM_DEP_MARKED="$COMP_DIR/cf_transformed_mem_interface_marked.mlir"

F_CFX="$COMP_DIR/cfx.mlir"
F_CFX_SCHEDULED="$COMP_DIR/cfx_sched.mlir"

# ============================================================================ #
# Compilation flow
# ============================================================================ #

# Reset output directory
rm -rf "$COMP_DIR" && mkdir -p "$COMP_DIR"

# ------------------------------------------------------------------------------
# NOTE:
# - ffp-contract will prevent clang from adding "fused add mul" into the IR
# We need to check out the clang language extensions carefully for more
# optimizations, e.g., loop unrolling:
# https://clang.llvm.org/docs/LanguageExtensions.html#loop-unrolling
# ------------------------------------------------------------------------------
$LLVM_BINS/clang -O0 -funroll-loops -S -emit-llvm "$F_C_SOURCE" \
  -I "$DYNAMATIC_DIR/include"  \
  -Xclang \
  -ffp-contract=off \
  -o "$F_CLANG"

exit_on_fail "Failed to compile to LLVM IR" \
  "Compiled to LLVM IR"

# ------------------------------------------------------------------------------
# NOTE:
# - When calling clang with "-ffp-contract=off", clang will bypass the
# "-disable-O0-optnone" flag and still adds "optnone" to the IR. This is a hacky
# way to ignore it
# - Clang always adds "noinline" to the IR.
# ------------------------------------------------------------------------------
sed -i "s/optnone//g" "$F_CLANG"
sed -i "s/noinline//g" "$F_CLANG"

# Strip information that we don't care (and mlir-translate also doesn't know how
# to handle it).
sed -i "s/^target datalayout = .*$//g" "$F_CLANG"
sed -i "s/^target triple = .*$//g" "$F_CLANG"

# ------------------------------------------------------------------------------
# NOTE:
# Here is a brief summary of what each llvm pass does:
# - inline: Inlines the function calls.
# - mem2reg: Promote allocas (allocate memory on the heap) into regs.
# - lowerswitch: Convert switch case into branches.
# - instcombine: combine operations. Needed to canonicalize a chain of GEPs.
# - loop-rotate: canonicalize loops to do-while loops
# - consthoist: moving constants around
# - simplifycfg: merge BBs
# - licm: moving the code outside the loops as much as possible
#
# NOTE: the optnone attribute sliently disables all the optimization in the
# passes; Check out the complete list: https://llvm.org/docs/Passes.html
#
# NOTE: There is a detailed overview of what passes are ran by `opt -O3`
# https://llvm.org/devmtg/2023-05/slides/Tutorial-May10/01-Popov-AWhirlwindTour-oftheLLVMOptimizer.pdf
# (Slide 26)
# ------------------------------------------------------------------------------

$LLVM_BINS/opt -S \
  -passes="inline,mem2reg,consthoist,instcombine<max-iterations=1000;no-use-loop-info>,function(loop-mssa(licm<no-allowspeculation>)),function(loop(loop-idiom,indvars,loop-deletion)),simplifycfg,loop-rotate,simplifycfg,sink,lowerswitch,simplifycfg,dce" \
  "$F_CLANG" \
  > "$F_CLANG_OPTIMIZED"
exit_on_fail "Failed to apply optimization to LLVM IR" \
  "Optimized LLVM IR"

# ------------------------------------------------------------------------------
# This pass uses polyhedral and alias analysis to determine the dependency
# between memory operations.
#
# Example:
# ======== histogram.ll =========
#  %2 = load float, ptr %arrayidx4, align 4, !handshake.name !5
#  ...
#  store float %add, ptr %arrayidx6, align 4, !handshake.name !6 !dest.ops !7
#  ...
# !5 = !{!"load1"}
# !6 = !{!"store!"}
# !7 = !{!5, !"1"} ; this means that the store must happen before the load, with
# a loop depth of 1
# ===============================
#
# ------------------------------------------------------------------------------
# NOTE:
# - without "--polly-process-unprofitable", polly ignores certain small loops
# - ArrayParititon pass currently breaks the SCoP analysis in Polly. Therefore,
# we need to first attach analysis results to memory ops and then apply memory
# bank partition.
$LLVM_BINS/opt -S \
  -load-pass-plugin "$DYNAMATIC_DIR/build/lib/MemDepAnalysis.so" \
  -passes="mem-dep-analysis" \
  --use-dependence-analysis \
  "$F_CLANG_OPTIMIZED" \
  > "$F_CLANG_OPTIMIZED_DEPENDENCY"
exit_on_fail "Failed to apply memory dependency analysis to LLVM IR" \
  "Applied memory dependency analysis to LLVM IR"

$LLVM_TO_STD_TRANSLATION_BIN \
  "$F_CLANG_OPTIMIZED_DEPENDENCY" \
  -function-name "$KERNEL_NAME" \
  -csource "$F_C_SOURCE" \
  -dynamatic-path "$DYNAMATIC_DIR" \
   -o "$F_CF"
exit_on_fail "Failed to convert to std dialect" \
  "Converted to std dialect"

# cf transformations (dynamatic)
# - "drop-unlist-functions": Dropping the functions that are not needed in HLS
# compilation.
# - "arith-reduce-strength": Convert muls to adds. "max-adder-depth-mul" limits
# the maximum length of the adder chain created via this pass.
$DYNAMATIC_OPT_BIN \
  "$F_CF" \
  --drop-unlisted-functions="function-names=$KERNEL_NAME" \
  --func-set-arg-names="source=$F_C_SOURCE" \
  --flatten-memref-row-major \
  --canonicalize \
  --arith-reduce-strength="max-adder-depth-mul=3" \
  --push-constants \
  > "$F_CF_TRANSFORMED"
exit_on_fail "Failed to apply CF transformations" \
  "Applied CF transformations"

"$DYNAMATIC_OPT_BIN" "$F_CF_TRANSFORMED" \
  --mark-memory-interfaces \
  > "$F_CF_DYN_TRANSFORMED_MEM_DEP_MARKED"
exit_on_fail "Failed to mark memory interfaces in cf" \
  "Marked memory accesses with the corresponding interfaces in cf"

# Convert CF to CFX dialect for static scheduling
"$DYNAMATIC_OPT_BIN" "$F_CF_DYN_TRANSFORMED_MEM_DEP_MARKED" \
  --cf-to-cfx \
  > "$F_CFX"
exit_on_fail "Failed to convert CF to CFX dialect" \
  "Convert CF to CFX dialect"

# Remove intermediate compilation outputs, keeping only cfx.mlir
find "$COMP_DIR" -type f ! -name "cfx.mlir" -delete

if [ "$GETTING_STARTED" = "getting-started" ]; then
  # Run the getting-started pass only
  "$DYNAMATIC_OPT_BIN" "$F_CFX" \
    --getting-started="timing-models=$DYNAMATIC_DIR/data-static/components-static.json" \
    > /dev/null
  exit_on_fail "Failed to run getting-started pass" \
    "Ran getting-started pass"


elif [ "$GETTING_STARTED" = "static-hls" ]; then
  # Add resource constraint option if resource_constraints is not empty
  if [ -n "$RESOURCE_CONSTRAINTS" ]; then
    RESOURCE_CONSTRAINTS_OPT="resource-constraints=$RESOURCE_CONSTRAINTS"
  else
    RESOURCE_CONSTRAINTS_OPT=""
  fi

  # Add maximum II option if specified
  if [ -n "$MAX_II" ]; then
    MAX_II_OPT="maximum-ii=$MAX_II"
  else
    MAX_II_OPT=""
  fi

  # Run static scheduling
  cd "$COMP_DIR"
  "$DYNAMATIC_OPT_BIN" "$F_CFX" \
    --static-scheduling="algorithm=$SCHEDULING_ALGORITHM solver=$MILP_SOLVER timing-models=$DYNAMATIC_DIR/data-static/components-static.json $MAX_II_OPT $RESOURCE_CONSTRAINTS_OPT resource-constraints=$DYNAMATIC_DIR/data-static/resource-constraints.json dump-logs" \
    > "$F_CFX_SCHEDULED"
  exit_on_fail "Failed to schedule CFX dialect" \
    "Scheduled CFX dialect"

else
  echo "Error: unrecognized mode '$GETTING_STARTED'. Expected 'getting-started' or 'static-hls'." >&2
  exit 1
fi
