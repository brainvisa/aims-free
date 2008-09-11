if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/ecat.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsecat-devel )
  add_library( aimsecat SHARED ${_sources} )
  set_property( TARGET aimsecat PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsecat aimsdata ecat+ )
  SOMA_INSTALL( TARGETS aimsecat
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(ecat.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsecat SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsecat
  aimsdata
  ecat+
  )
  
  
  INSTALL(TARGETS aimsecat DESTINATION lib/)

endif( soma-infra_FOUND )
