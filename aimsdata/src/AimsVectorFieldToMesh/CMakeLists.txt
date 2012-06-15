
BRAINVISA_ADD_EXECUTABLE(AimsVectorFieldToMesh
vectorFieldToMesh.cc
)

TARGET_LINK_LIBRARIES(AimsVectorFieldToMesh
aimsdata
)

INSTALL(TARGETS AimsVectorFieldToMesh DESTINATION bin/)
