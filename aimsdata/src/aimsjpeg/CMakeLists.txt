if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/jpeg.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsjpeg-devel )
  add_library( aimsjpeg SHARED ${_sources} )
  set_property( TARGET aimsjpeg PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsjpeg aimsdata ${JPEG_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimsjpeg
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )
  
  getFromPro(jpeg.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsjpeg SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsjpeg
  aimsdata
  ${JPEG_LIBRARIES}
  )
  
  
  INSTALL(TARGETS aimsjpeg DESTINATION lib/)

endif( soma-infra_FOUND )
