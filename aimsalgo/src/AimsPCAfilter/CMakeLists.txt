
ADD_EXECUTABLE(AimsPCAfilter
individu.cc
main.cc
)


TARGET_LINK_LIBRARIES(AimsPCAfilter
aimsalgo
)


INSTALL(TARGETS AimsPCAfilter DESTINATION bin/)
