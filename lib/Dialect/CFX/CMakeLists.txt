add_dynamatic_dialect_library(DynamaticCFX
  CFX.cpp

  DEPENDS
  MLIRCFXIncGen

  LINK_LIBS PUBLIC
  MLIRIR
)