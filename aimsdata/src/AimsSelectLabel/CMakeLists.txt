
BRAINVISA_ADD_EXECUTABLE(AimsSelectLabel
selectLabel.cc
)

TARGET_LINK_LIBRARIES(AimsSelectLabel
aimsdata
)

INSTALL(TARGETS AimsSelectLabel DESTINATION bin/)
