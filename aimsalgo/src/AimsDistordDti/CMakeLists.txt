
ADD_EXECUTABLE(AimsDistordDti
interpolator.cc
main.cc
)

TARGET_LINK_LIBRARIES(AimsDistordDti
aimsalgo
)

INSTALL(TARGETS AimsDistordDti DESTINATION bin/)
