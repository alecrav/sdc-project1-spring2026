add_dynamatic_library(DynamaticStaticScheduling
  ASAPScheduling.cpp
  ALAPScheduling.cpp
  GettingStartedPass.cpp
  PipelinedScheduling.cpp
  ResourceConstraints.cpp
  SchedulingIO.cpp
  StaticTimingConfig.cpp
  StaticSchedulingILP.cpp
  StaticSchedulingPass.cpp

  DEPENDS
  DynamaticTransformsPassIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRMemRefDialect
  MLIRFuncDialect
  MLIRSupport
  MLIRTransformUtils
  DynamaticHandshake
  DynamaticSupport
  DynamaticExperimentalSupport

  )

if (GUROBI_FOUND)
  target_link_libraries(DynamaticStaticScheduling
    PUBLIC
    ${GUROBI_CXX_LIBRARY}
    ${GUROBI_LIBRARY}
  )
endif()

if (CBC_FOUND AND CLPCORE_FOUND AND OSI_FOUND)
target_link_libraries(DynamaticStaticScheduling
  PUBLIC
  ${CBC_LIBRARIES}
  ${CLPCORE_LIBRARIES}
  ${OSI_LIBRARIES}
)
endif()
