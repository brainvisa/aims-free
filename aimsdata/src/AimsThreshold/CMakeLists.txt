
BRAINVISA_ADD_EXECUTABLE(AimsThreshold
main.cc
)

TARGET_LINK_LIBRARIES(AimsThreshold
aimsdata
)

INSTALL(TARGETS AimsThreshold DESTINATION bin/)
