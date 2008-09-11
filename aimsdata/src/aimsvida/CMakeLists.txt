if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/vida.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsvida-devel )
  add_library( aimsvida SHARED ${_sources} )
  set_property( TARGET aimsvida PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsvida aimsdata vidaIO )
  SOMA_INSTALL( TARGETS aimsvida
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(vida.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsvida SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsvida
  aimsdata
  vidaIO
  )
  
  
  INSTALL(TARGETS aimsvida DESTINATION lib/)

endif( soma-infra_FOUND )
