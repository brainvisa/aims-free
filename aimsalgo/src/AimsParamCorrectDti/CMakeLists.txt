
BRAINVISA_ADD_EXECUTABLE(AimsParamCorrectDti
interpolator.cc
main.cc
)

TARGET_LINK_LIBRARIES(AimsParamCorrectDti
aimsalgo
)

INSTALL(TARGETS AimsParamCorrectDti DESTINATION bin/)
