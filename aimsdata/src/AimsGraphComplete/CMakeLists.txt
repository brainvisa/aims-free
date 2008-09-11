
ADD_EXECUTABLE(AimsGraphComplete
graphComplete.cc
)


TARGET_LINK_LIBRARIES(AimsGraphComplete
aimsdata
)

INSTALL(TARGETS AimsGraphComplete DESTINATION bin/)
