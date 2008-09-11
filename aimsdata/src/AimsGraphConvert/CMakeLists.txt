
ADD_EXECUTABLE(AimsGraphConvert
graphConvert.cc
)


TARGET_LINK_LIBRARIES(AimsGraphConvert
aimsdata
)

INSTALL(TARGETS AimsGraphConvert DESTINATION bin/)
