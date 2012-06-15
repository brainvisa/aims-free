
BRAINVISA_ADD_EXECUTABLE(AimsInvertTransformation
inverttrans.cc
)

TARGET_LINK_LIBRARIES(AimsInvertTransformation
aimsdata
)

INSTALL(TARGETS AimsInvertTransformation DESTINATION bin/)
