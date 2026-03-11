add_subdirectory(Handshake)
add_subdirectory(HW)
add_subdirectory(CFX)

add_library(DynamaticDialects INTERFACE)
target_link_libraries(DynamaticDialects
  INTERFACE
    DynamaticHandshake
    DynamaticHW
    DynamaticCFX
)