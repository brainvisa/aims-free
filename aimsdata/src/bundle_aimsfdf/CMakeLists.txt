if( soma-infra_FOUND )

  add_library( bundle_aimsfdf MODULE bundle/fdfbundle.cc )
#   set_property( TARGET bundle_aimsfdf PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsfdf PROPERTY OUTPUT_NAME aimsfdf.${${PROJECT_NAME}_VERSION} )
  target_link_libraries( bundle_aimsfdf aimsfdf )
  SOMA_INSTALL( TARGETS bundle_aimsfdf
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsfdf MODULE
  bundle/fdfbundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsfdf
  aimsfdf
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsfdf
  PROPERTIES OUTPUT_NAME aimsfdf
  )
  
  INSTALL(TARGETS bundle_aimsfdf DESTINATION lib/)

endif( soma-infra_FOUND )
