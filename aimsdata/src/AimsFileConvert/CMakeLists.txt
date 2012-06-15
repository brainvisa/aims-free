
BRAINVISA_ADD_EXECUTABLE(AimsFileConvert
fileConvert.cc
)


TARGET_LINK_LIBRARIES(AimsFileConvert
aimsdata
)

INSTALL(TARGETS AimsFileConvert DESTINATION bin/)
