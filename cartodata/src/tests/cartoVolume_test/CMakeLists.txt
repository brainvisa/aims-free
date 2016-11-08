
BRAINVISA_ADD_EXECUTABLE(cartoVolume_test
volumetest.cc 
HELP_GENERATE FALSE
)

TARGET_LINK_LIBRARIES(cartoVolume_test
cartodata
)

BRAINVISA_INSTALL( TARGETS cartoVolume_test
              DESTINATION bin
              COMPONENT ${PROJECT_NAME}-test )

add_test( cartoVolume_test "${CMAKE_BINARY_DIR}/bin/bv_env_test" cartoVolume_test )

