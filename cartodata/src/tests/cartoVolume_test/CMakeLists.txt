
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

brainvisa_add_test( cartoVolume_test cartoVolume_test )

