if( soma-infra_FOUND )

  add_library( bundle_aimsminc MODULE bundle/mincbundle.cc )
  set_property( TARGET bundle_aimsminc PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsminc PROPERTY OUTPUT_NAME aimsminc )
  target_link_libraries( bundle_aimsminc aimsminc )
  SOMA_INSTALL( TARGETS bundle_aimsminc
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsminc MODULE
  bundle/mincbundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsminc
  aimsminc
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsminc
  PROPERTIES OUTPUT_NAME aimsminc
  )
  
  INSTALL(TARGETS bundle_aimsminc DESTINATION lib/)

endif( soma-infra_FOUND )
