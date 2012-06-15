
BRAINVISA_ADD_EXECUTABLE(cartoVolume_test
volumetest.cc 
HELP_GENERATE FALSE
)

TARGET_LINK_LIBRARIES(cartoVolume_test
cartodata
)
