
BRAINVISA_ADD_EXECUTABLE(AimsGraphMerge
graphMerge.cc
)

TARGET_LINK_LIBRARIES(AimsGraphMerge
aimsdata
)

INSTALL(TARGETS AimsGraphMerge DESTINATION bin/)
