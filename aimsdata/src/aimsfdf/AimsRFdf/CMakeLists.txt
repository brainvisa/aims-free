if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO( rfdf.pro SOURCES _sources )
  add_executable( AimsRfdf ${_sources} )
  target_link_libraries( AimsRfdf aimsdata aimsfdf )
  SOMA_INSTALL( TARGETS AimsRfdf
                DESTINATION bin
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(rfdf.pro HEADERS SOURCES)
  
  add_executable(AimsRfdf
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(AimsRfdf
  aimsdata
  aimsfdf
  )
  
  
  INSTALL_TARGETS(/bin AimsRfdf)

endif( soma-infra_FOUND )
