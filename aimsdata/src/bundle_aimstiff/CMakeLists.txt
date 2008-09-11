if( soma-infra_FOUND )

  add_library( bundle_aimstiff MODULE bundle/tiffbundle.cc )
  set_property( TARGET bundle_aimstiff PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimstiff PROPERTY OUTPUT_NAME aimstiff )
  target_link_libraries( bundle_aimstiff aimstiff )
  SOMA_INSTALL( TARGETS bundle_aimstiff
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimstiff MODULE
  bundle/tiffbundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimstiff
  aimstiff
  )
  
  SET_TARGET_PROPERTIES(bundle_aimstiff
  PROPERTIES OUTPUT_NAME aimstiff
  )
  
  INSTALL(TARGETS bundle_aimstiff DESTINATION lib/)

endif( soma-infra_FOUND )
