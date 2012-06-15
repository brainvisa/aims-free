
BRAINVISA_ADD_EXECUTABLE(AimsOverVolume
main.cc
)

TARGET_LINK_LIBRARIES(AimsOverVolume
aimsdata
)

INSTALL(TARGETS AimsOverVolume DESTINATION bin/)
