BRAINVISA_GET_FILE_LIST_FROM_PRO( rtiff.pro SOURCES _sources )
add_executable( AimsRTiff ${_sources} )
target_link_libraries( AimsRTiff aimsdata aimstiff )
BRAINVISA_INSTALL( TARGETS AimsRTiff
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
