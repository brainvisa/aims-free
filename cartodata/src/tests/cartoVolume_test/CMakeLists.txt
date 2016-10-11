
BRAINVISA_ADD_EXECUTABLE(cartoVolume_test
volumetest.cc 
HELP_GENERATE FALSE
)

TARGET_LINK_LIBRARIES(cartoVolume_test
cartodata
)

add_test( cartoVolume_test "${CMAKE_BINARY_DIR}/bin/bv_env${CMAKE_EXECUTABLE_SUFFIX}" cartoVolume_test )

