if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsdata.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsdata-devel )
  add_library( aimsdata SHARED ${_sources} )
  set_property( TARGET aimsdata PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsdata graph cartodata )
  SOMA_INSTALL( TARGETS aimsdata
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(aimsdata.pro HEADERS SOURCES)
  
  #IF(APPLE)
  #  SET(SOURCES
  #  ${SOURCES}
  #  io/fileFormat_graph2.cc
  #  )
  
  #  SET(HEADERS
  #  ${HEADERS}
  #  io/fileFormat_graph2.h
  #  )
  #ENDIF(APPLE)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsdata SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsdata
  graph
  cartodata
  )
  
  SET_TARGET_PROPERTIES(aimsdata PROPERTIES ${BrainVISA_LIBRARY_PROPERTIES})
  
  INSTALL(TARGETS aimsdata DESTINATION lib/)

endif( soma-infra_FOUND )
