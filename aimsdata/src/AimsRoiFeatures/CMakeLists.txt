
BRAINVISA_ADD_EXECUTABLE(AimsRoiFeatures
AimsRoiFeatures.cc
)

TARGET_LINK_LIBRARIES(AimsRoiFeatures
aimsdata
)

INSTALL(TARGETS AimsRoiFeatures DESTINATION bin/)
