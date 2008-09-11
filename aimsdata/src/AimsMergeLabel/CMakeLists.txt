
ADD_EXECUTABLE(AimsMergeLabel
mergeLabel.cc
)

TARGET_LINK_LIBRARIES(AimsMergeLabel
aimsdata
)

INSTALL(TARGETS AimsMergeLabel DESTINATION bin/)
