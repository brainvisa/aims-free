
BRAINVISA_ADD_EXECUTABLE(AimsMeshTransform
main.cc
)

TARGET_LINK_LIBRARIES(AimsMeshTransform
aimsdata
)

INSTALL(TARGETS AimsMeshTransform DESTINATION bin/)
