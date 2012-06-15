
BRAINVISA_ADD_EXECUTABLE(AimsAverage
average.cc
)

TARGET_LINK_LIBRARIES(AimsAverage
aimsdata
)

INSTALL(TARGETS AimsAverage DESTINATION bin/)
