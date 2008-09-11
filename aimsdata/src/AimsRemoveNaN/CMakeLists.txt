
ADD_EXECUTABLE(AimsRemoveNaN
AimsRemoveNaN.cc
)

TARGET_LINK_LIBRARIES(AimsRemoveNaN
aimsdata
)

INSTALL(TARGETS AimsRemoveNaN DESTINATION bin/)
