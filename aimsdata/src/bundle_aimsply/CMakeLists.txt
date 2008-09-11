if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/bundleply.pro "HEADERS" _headers "SOURCES" _sources )
  add_library( bundle_aimsply MODULE ${_sources} )
  set_property( TARGET bundle_aimsply PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsply PROPERTY OUTPUT_NAME aimsply )
  target_link_libraries( bundle_aimsply aimsply )
  SOMA_INSTALL( TARGETS bundle_aimsply
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsply MODULE
  bundle/plybundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsply
  aimsply
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsply
  PROPERTIES OUTPUT_NAME aimsply
  )
  
  INSTALL(TARGETS bundle_aimsply DESTINATION lib/)

endif( soma-infra_FOUND )
