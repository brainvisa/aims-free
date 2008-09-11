if( soma-infra_FOUND )

  add_library( bundle_aimsdicom MODULE bundle/dicombundle.cc )
  set_property( TARGET bundle_aimsdicom PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsdicom PROPERTY OUTPUT_NAME aimsdicom )
  target_link_libraries( bundle_aimsdicom aimsdicom )
  SOMA_INSTALL( TARGETS bundle_aimsdicom
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsdicom MODULE
  bundle/dicombundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsdicom
  aimsdicom
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsdicom
  PROPERTIES OUTPUT_NAME aimsdicom
  )
  
  INSTALL(TARGETS bundle_aimsdicom DESTINATION lib/)

endif( soma-infra_FOUND )
