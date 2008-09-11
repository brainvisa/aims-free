
ADD_EXECUTABLE(AimsMerge2Rgb
merge2Rgb.cc
)

TARGET_LINK_LIBRARIES(AimsMerge2Rgb
aimsdata
)

INSTALL(TARGETS AimsMerge2Rgb DESTINATION bin/)
