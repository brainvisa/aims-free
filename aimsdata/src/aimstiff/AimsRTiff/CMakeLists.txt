if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO( rtiff.pro SOURCES _sources )
  add_executable( AimsRTiff ${_sources} )
  target_link_libraries( AimsRTiff aimsdata aimstiff )
  SOMA_INSTALL( TARGETS AimsRTiff
                DESTINATION bin
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(rtiff.pro HEADERS SOURCES)
  
  add_executable(AimsRTiff
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(AimsRTiff
  aimsdata
  aimstiff
  )
  
  
  INSTALL_TARGETS(/bin AimsRTiff)

endif( soma-infra_FOUND )
