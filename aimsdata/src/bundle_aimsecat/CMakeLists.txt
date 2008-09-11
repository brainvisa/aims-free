if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/bundleecat.pro "HEADERS" _headers "SOURCES" _sources )
  add_library( bundle_aimsecat MODULE ${_sources} )
  set_property( TARGET bundle_aimsecat PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsecat PROPERTY OUTPUT_NAME aimsecat )
  target_link_libraries( bundle_aimsecat aimsecat )
  SOMA_INSTALL( TARGETS bundle_aimsecat
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsecat MODULE
  bundle/ecatbundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsecat
  aimsecat
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsecat
  PROPERTIES OUTPUT_NAME aimsecat
  )
  
  INSTALL(TARGETS bundle_aimsecat DESTINATION lib/)

endif( soma-infra_FOUND )
