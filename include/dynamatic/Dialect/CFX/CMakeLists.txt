add_dynamatic_dialect(CFX cfx)

set(LLVM_TARGET_DEFINITIONS CFX.td)

mlir_tablegen(CFXEnums.h.inc -gen-enum-decls)
mlir_tablegen(CFXEnums.cpp.inc -gen-enum-defs)

add_public_tablegen_target(MLIRCFXEnumsIncGen)
add_dependencies(dynamatic-headers MLIRCFXEnumsIncGen)

mlir_tablegen(CFXAttributes.h.inc -gen-attrdef-decls)
mlir_tablegen(CFXAttributes.cpp.inc -gen-attrdef-defs)
add_public_tablegen_target(MLIRCFXAttributesIncGen)
add_dependencies(dynamatic-headers MLIRCFXAttributesIncGen)