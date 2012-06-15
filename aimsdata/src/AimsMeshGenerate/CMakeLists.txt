
BRAINVISA_ADD_EXECUTABLE(AimsMeshGenerate
meshGenerate.cc
)

TARGET_LINK_LIBRARIES(AimsMeshGenerate
aimsdata
)

INSTALL(TARGETS AimsMeshGenerate DESTINATION bin/)
