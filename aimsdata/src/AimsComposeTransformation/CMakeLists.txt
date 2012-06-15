
BRAINVISA_ADD_EXECUTABLE(AimsComposeTransformation
composetrans.cc
)

TARGET_LINK_LIBRARIES(AimsComposeTransformation
aimsdata
)

INSTALL(TARGETS AimsComposeTransformation DESTINATION bin/)
