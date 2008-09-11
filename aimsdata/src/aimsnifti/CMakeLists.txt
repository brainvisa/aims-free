if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/nifti.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsnifti-devel )
  include_directories( "${CMAKE_BINARY_DIR}/include/aims/io/znzlib" )
  add_library( aimsnifti SHARED ${_sources} )
  set_property( TARGET aimsnifti PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsnifti aimsdata ${NIFTI_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimsnifti
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(nifti.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsnifti SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsnifti
  aimsdata
  ${NIFTI_LIBRARIES}
  )
  
  
  INSTALL(TARGETS aimsnifti DESTINATION lib/)

endif( soma-infra_FOUND )
