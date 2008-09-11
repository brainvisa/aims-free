if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/minc.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsminc-devel )
  add_library( aimsminc SHARED ${_sources} )
  set_property( TARGET aimsminc PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsminc aimsdata ${MINC_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimsminc
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(minc.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsminc SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsminc
  aimsdata
  ${MINC_LIBRARIES}
  )
  
  
  INSTALL(TARGETS aimsminc DESTINATION lib/)

endif( soma-infra_FOUND )
