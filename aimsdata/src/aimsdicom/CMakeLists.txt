if( soma-infra_FOUND )

  include_directories( ${DCMTK_INCLUDE_DIR} )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/dicom.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsdicom-devel )
  add_library( aimsdicom SHARED ${_sources} )
  set_property( TARGET aimsdicom PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsdicom aimsdata ${DCMTK_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimsdicom
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(dicom.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsdicom SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsdicom
  aimsdata
  ${DCMTK_LIBRARIES}
  )
  
  
  INSTALL(TARGETS aimsdicom DESTINATION lib/)

endif( soma-infra_FOUND )
