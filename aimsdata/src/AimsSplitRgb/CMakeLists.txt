
ADD_EXECUTABLE(AimsSplitRgb
splitRgb.cc
)

TARGET_LINK_LIBRARIES(AimsSplitRgb
aimsdata
)

INSTALL(TARGETS AimsSplitRgb DESTINATION bin/)
