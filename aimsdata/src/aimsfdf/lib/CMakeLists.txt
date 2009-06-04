if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aims-fdf.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsfdf-devel )
  add_library( aimsfdf SHARED ${_sources} )
  set_property( TARGET aimsfdf PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsfdf aimsdata ${fdf_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimsfdf
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(lib-aims-fdf.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsfdf SHARED
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsfdf
  aimsdata
  ${fdf_LIBRARIES}
  )
  
  
  INSTALL(TARGETS aimsfdf DESTINATION lib/)

endif( soma-infra_FOUND )
