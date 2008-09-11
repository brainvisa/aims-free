if( soma-infra_FOUND )

  add_library( bundle_aimsnifti MODULE bundle/niftibundle.cc )
  set_property( TARGET bundle_aimsnifti PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsnifti PROPERTY OUTPUT_NAME aimsnifti )
  target_link_libraries( bundle_aimsnifti aimsnifti )
  SOMA_INSTALL( TARGETS bundle_aimsnifti
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )
  
  ADD_LIBRARY(bundle_aimsnifti MODULE
  bundle/niftibundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsnifti
  aimsnifti
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsnifti
  PROPERTIES OUTPUT_NAME aimsnifti
  )
  
  INSTALL(TARGETS bundle_aimsnifti DESTINATION lib/)

endif( soma-infra_FOUND )
