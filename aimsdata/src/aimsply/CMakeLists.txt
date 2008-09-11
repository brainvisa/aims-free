if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsply.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsply-devel )
  add_library( aimsply SHARED ${_sources} )
  set_property( TARGET aimsply PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsply aimsdata )
  SOMA_INSTALL( TARGETS aimsply
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(aimsply.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsply SHARED 
  io/plyformat.cc
  io/plyheader.cc
  plugin/plyplugin.cc
  rply/rply.c
  )
  
  TARGET_LINK_LIBRARIES(aimsply
  aimsdata
  )
  
  
  INSTALL(TARGETS aimsply DESTINATION lib/)

endif( soma-infra_FOUND )
