
ADD_EXECUTABLE(AimsMassCenter
massCenter.cc
)

TARGET_LINK_LIBRARIES(AimsMassCenter
aimsdata
)

INSTALL(TARGETS AimsMassCenter DESTINATION bin/)
