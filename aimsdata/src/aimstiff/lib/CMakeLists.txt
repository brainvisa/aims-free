if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aims-tiff.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimstiff-devel )
  add_library( aimstiff SHARED ${_sources} )
  set_property( TARGET aimstiff PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimstiff aimsdata ${TIFF_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimstiff
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(lib-aims-tiff.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimstiff SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimstiff
  aimsdata
  ${TIFF_LIBRARIES}
  )
  
  
  INSTALL(TARGETS aimstiff DESTINATION lib/)

endif( soma-infra_FOUND )
