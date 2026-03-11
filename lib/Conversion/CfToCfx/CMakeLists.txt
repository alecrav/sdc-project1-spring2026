add_dynamatic_library(DynamaticCfToCfx
  CfToCfx.cpp

  DEPENDS
  DynamaticConversionPassIncGen

  LINK_LIBS PUBLIC
  DynamaticSupport
  DynamaticCFX
  DynamaticTransforms
  MLIRIR
  MLIRArithDialect
  MLIRPass
  MLIRControlFlowDialect
  MLIRFuncDialect
)