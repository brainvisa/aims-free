
ADD_EXECUTABLE(AimsConnectComp
main.cc
)

TARGET_LINK_LIBRARIES(AimsConnectComp
aimsdata
)

INSTALL(TARGETS AimsConnectComp DESTINATION bin/)
