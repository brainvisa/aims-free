
BRAINVISA_ADD_EXECUTABLE(AimsSubVolume
subVolume.cc
)

TARGET_LINK_LIBRARIES(AimsSubVolume
aimsdata
)

INSTALL(TARGETS AimsSubVolume DESTINATION bin/)
