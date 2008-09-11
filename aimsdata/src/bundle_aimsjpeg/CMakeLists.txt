if( soma-infra_FOUND )

  add_library( bundle_aimsjpeg MODULE bundle/jpegbundle.cc )
  set_property( TARGET bundle_aimsjpeg PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsjpeg PROPERTY OUTPUT_NAME aimsjpeg )
  target_link_libraries( bundle_aimsjpeg aimsjpeg )
  SOMA_INSTALL( TARGETS bundle_aimsjpeg
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsjpeg MODULE
  bundle/jpegbundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsjpeg
  aimsjpeg
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsjpeg
  PROPERTIES OUTPUT_NAME aimsjpeg
  )
  
  INSTALL(TARGETS bundle_aimsjpeg DESTINATION lib/)

endif( soma-infra_FOUND )
