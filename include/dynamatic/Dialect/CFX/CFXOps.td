//===- CFXOps.td - CFX operation definitions ------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines CFX operations in tablegen.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_DIALECT_CFX_CFX_OPS_TD
#define DYNAMATIC_DIALECT_CFX_CFX_OPS_TD

include "mlir/IR/BuiltinTypes.td"
include "mlir/IR/BuiltinAttributeInterfaces.td"
include "dynamatic/Dialect/CFX/CFX.td"
include "mlir/Interfaces/InferTypeOpInterface.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/IR/OpBase.td"
include "mlir/IR/RegionKindInterface.td"
include "mlir/Interfaces/FunctionInterfaces.td"
include "mlir/IR/EnumAttr.td"

// We need to replace the func.func op
// since it has dominance rules which are
// incompatible with phi operations
def CFX_FuncOp : Op<CFX_Dialect, "func", [
  IsolatedFromAbove,    // Enforce scoping on the function
  FunctionOpInterface,  // Automatic function signature and calling handling
  Symbol,               // Automatic function name handling
  RegionKindInterface   // Used to relax dominance rules to enable phis
]> {
  // Parameters of the function op: name and type
  let arguments = (ins
    SymbolNameAttr:$sym_name,
    TypeAttrOf<FunctionType>:$function_type,
    // Per-argument and per-result attribute arrays (required by
    // function_interface_impl::printFunctionOp / parseFunctionOp)
    OptionalAttr<DictArrayAttr>:$arg_attrs,
    OptionalAttr<DictArrayAttr>:$res_attrs
  );

  // Function has a body
  let regions = (region AnyRegion:$body);

  // Define what happens when someone tries to add
  // a cfx.func to the IR
  //
  // This is identical to the default behaviour
  // (copy the parameters into the right place)
  //
  // apart from addRegion, which adds a body to the func op
  let builders = [
    OpBuilder<(ins "::mlir::StringAttr":$name,
                  "::mlir::TypeAttr":$type), [{
      $_state.addAttribute("sym_name", name);
      $_state.addAttribute("function_type", type);

      $_state.addRegion();
    }]>
  ];

  // Custom assembly format: function arguments are printed as named SSA values
  // in the function signature, matching func.func. The print/parse
  // implementation uses function_interface_impl helpers — see CFX.cpp.
  let hasCustomAssemblyFormat = 1;

  let extraClassDeclaration = [{
    // Required by the FunctionOpInterface
    ArrayRef<Type> getArgumentTypes() {
      return getFunctionType().getInputs();
    }

    // Required by the FunctionOpInterface
    ArrayRef<Type> getResultTypes() {
      return getFunctionType().getResults();
    }

    // Required by the FunctionOpInterface
    ::mlir::Region *getCallableRegion() { return &getBody(); }

    // Relax dominance rules to enable phis
    static ::mlir::RegionKind getRegionKind(unsigned) {
      return ::mlir::RegionKind::Graph;
    }

    // The cfx.func op is responsible for
    // storing the CFG
    // These operations are how it does so
    static int64_t getBB(::mlir::Operation *op);
    static void setBB(::mlir::Operation *op, int64_t bbID);

    // Get the operations in the function grouped by their basic block
    // Example:
    // cfx::FuncOp funcOp;
    // for (auto block : getCFXBasicBlocks) {
    //   for (auto * op : block) {
    //     // Do something with the op
    //   }
    // }
    // For more details, check out dynamatic/Support/CFX.h
    llvm::SmallVector<CFXBasicBlock> getCFXBasicBlocks();

    // Get the back edges that correspond to self-loops in CFG
    llvm::SetVector<std::pair<Operation *, Operation *>> getSelfLoopBackEdgesInCDFG();

    // Set the scheduling variable value for all operations in the function
    mlir::LogicalResult addSchedulingAttributes(llvm::MapVector<Operation *, unsigned> &result);

    // Set the II value for basic block pipelined scheduling
    void setInitiationIntervalForBlock(uint32_t bbID, unsigned ii);

    // Set the II value for basic block pipelined scheduling
    void setInitiationIntervalForBlock(const CFXBasicBlock &bb, unsigned ii);

    // Get the II value for basic block pipelined scheduling
    std::optional<unsigned> getInitiationIntervalForBlock(uint32_t bbID);

    // Get the II value for basic block pipelined scheduling
    std::optional<unsigned> getInitiationIntervalForBlock(const CFXBasicBlock &bb);
  }];
}

// i0 is a zero-width integer used as a control-only token.
// The start signal and gating operands on cfx.constant / cfx.return
// use this type.
def I0Type : Type<CPred<"$_self.isInteger(0)">, "i0"> {
  let builderCall = "$_builder.getIntegerType(0)";
}

// Replaces func.return.
// func.return must be inside func.func, and CfToCfx converts all
// func.funcs to cfx.funcs, so a replacement is needed.
// Identical to func.return except for an additional start signal operand.
// Return ops exist in every exit block, so carrying the start signal
// here guarantees it is SSA maximized to every block.
def CFX_ReturnOp : Op<CFX_Dialect, "return", [
  HasParent<"dynamatic::cfx::FuncOp">,
  Terminator
]> {
  let arguments = (ins I0Type:$start, Variadic<AnyType>:$values);
  let assemblyFormat = [{
    `start` `=` $start `,` $values attr-dict (`:` type($values)^)?
  }];
}

// CFX phi op with any number of inputs, one output
// SameOperandsAndResultType means all edges must be the same type
def CFX_PhiOp : Op<CFX_Dialect, "phi", [SameOperandsAndResultType]> {
  let summary = "CFX phi";

  let arguments = (ins
    Variadic<AnyType>:$inputs
  );

  let results = (outs
    AnyType:$result
  );

  let assemblyFormat = [{
    $inputs attr-dict `:` type($inputs) `->` type($result)
  }];
}

// CFX phi op with one input, one output
// SameOperandsAndResultType means all edges must be the same type
def CFX_BranchOp : Op<CFX_Dialect, "br", [SameOperandsAndResultType]> {
  let summary = "CFX branch";

  let arguments = (ins
    AnyType:$operand
  );

  let results = (outs
    AnyType:$result
  );

  let assemblyFormat = [{
    $operand attr-dict `:` type($operand)
  }];
}


// Conditional branches connect one operand to one successor BB
// So they must store the polarity of that successor edge
def CFX_BranchPolarity : I32EnumAttr<
  "BranchPolarity",
  "",
  [
    I32EnumAttrCase<"when_true",  0>,
    I32EnumAttrCase<"when_false", 1>
  ]
> {
  let cppNamespace = "::dynamatic::cfx";
}

// CFX phi op with two inputs (condition and data), one output
// Custom verifier to ensure that input and result are the same type
// Has CFX_BranchPolarity:$polarity attribute to store
// polarity of successor edge
def CFX_CondBranchOp : Op<CFX_Dialect, "cond_br"> {
  let summary = "CFX value-based conditional branch";

  let arguments = (ins
    I1:$cond,
    AnyType:$input,
    CFX_BranchPolarity:$polarity
  );

  let results = (outs
    AnyType:$result
  );

  let hasVerifier = 1;

  let assemblyFormat = [{
    `cond` `=` $cond`[` $polarity `]` `,` `input` `=` $input
    attr-dict `:` type($cond) `,` type($input) `->` type($result)
  }];
}

def CFX_StartOp : Op<CFX_Dialect, "start"> {
  let summary = "Produces the i0 start signal for the entry block.";
  let description = [{
    Placed once in the entry basic block. The result is threaded through
    cfx.br outputs to gate cfx.constant operations in non-entry blocks,
    ensuring no constant fires before control reaches its block.
  }];
  let results = (outs I0Type:$result);
  let assemblyFormat = "attr-dict";
  let builders = [
    OpBuilder<(ins), [{
      $_state.addTypes($_builder.getIntegerType(0));
    }]>
  ];
}

def CFX_ConstantOp : Op<CFX_Dialect, "constant"> {
  let summary = "A constant gated by the start signal.";
  let description = [{
    Produces a compile-time constant, gated by the i0 start signal so it
    cannot fire before control flow reaches its block. The value attribute
    follows the same convention as arith.constant.
  }];
  let arguments = (ins I0Type:$start, TypedAttrInterface:$value);
  let results = (outs AnyType:$result);
  let assemblyFormat = "$start $value attr-dict `:` type($result)";
  let builders = [
    OpBuilder<(ins "::mlir::Value":$start, "::mlir::TypedAttr":$value), [{
      $_state.addOperands(start);
      $_state.addAttribute("value", value);
      $_state.addTypes(value.getType());
    }]>
  ];
}

// Attribute to store the scheduling variable value for each operation
def CFX_SchedulingVarAttr :  AttrDef<CFX_Dialect, "SchedVar"> {
  let summary = "CFX scheduling variable attribute";
  let description = [{
    This attribute is used to store the scheduling variable value for each operation in the CFX dialect.
  }];

  let mnemonic = "sched_var";

  let parameters = (ins
    "uint32_t":$sched_var
  );

  let assemblyFormat = "`<` $sched_var `>`";
}

#endif // DYNAMATIC_DIALECT_CFX_CFX_OPS_TD