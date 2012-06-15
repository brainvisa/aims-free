
BRAINVISA_ADD_EXECUTABLE(AimsTestArgs_example
AimsTestArgs.cc
)

TARGET_LINK_LIBRARIES(AimsTestArgs_example
aimsdata
)

INSTALL(TARGETS AimsTestArgs_example DESTINATION bin/)
