//===- CFX.td - CFX dialect definition -------------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This is the top-level TableGen file for the CFX dialect.
//
//===----------------------------------------------------------------------===//

#ifndef DYNAMATIC_DIALECT_CFX_CFX_TD
#define DYNAMATIC_DIALECT_CFX_CFX_TD

include "mlir/IR/OpBase.td"

def CFX_Dialect : Dialect {
  let name = "cfx";
  let cppNamespace = "::dynamatic::cfx";

  let summary = "CFX dialect";
  let description = [{
    This dialect defines the `cfx` dialect.
  }];

  // Not quite sure how properties work
  // They somehow replace attributes
  // but I know how attributes work so
  // using them instead
  let usePropertiesForAttributes = 0;
  let useDefaultAttributePrinterParser = 1;
}

include "dynamatic/Dialect/CFX/CFXOps.td"

#endif // DYNAMATIC_DIALECT_CFX_CFX_TD