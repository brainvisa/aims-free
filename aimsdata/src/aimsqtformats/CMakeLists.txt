if( soma-infra_FOUND )

  add_definitions( ${QT_DEFINITIONS} )
  include_directories( ${QT_INCLUDE_DIR} )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/qtformats.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsqtformats-devel )
  add_library( aimsqtformats SHARED ${_sources} )
  set_property( TARGET aimsqtformats PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsqtformats aimsdata ${QT_LIBRARIES} )
  SOMA_INSTALL( TARGETS aimsqtformats
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  getFromPro(qtformats.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  add_library(aimsqtformats SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsqtformats
  aimsdata
  ${QT_LIBRARIES}
  )
  
  
INSTALL(TARGETS aimsqtformats DESTINATION lib/)

endif( soma-infra_FOUND )
