if( soma-infra_FOUND )

  add_library( bundle_aimsvida MODULE bundle/vidabundle.cc )
  set_property( TARGET bundle_aimsvida PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsvida PROPERTY OUTPUT_NAME aimsvida )
  target_link_libraries( bundle_aimsvida aimsvida )
  SOMA_INSTALL( TARGETS bundle_aimsvida
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsvida MODULE
  bundle/vidabundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsvida
  aimsvida
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsvida
  PROPERTIES OUTPUT_NAME aimsvida
  )
  
  INSTALL(TARGETS bundle_aimsvida DESTINATION lib/)

endif( soma-infra_FOUND )
