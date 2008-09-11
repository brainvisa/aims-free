if( soma-infra_FOUND )

  add_library( bundle_aimsqtformats MODULE bundle/qtbundle.cc )
  set_property( TARGET bundle_aimsqtformats PROPERTY VERSION ${aimsdata_VERSION} )
  set_property( TARGET bundle_aimsqtformats PROPERTY OUTPUT_NAME aimsqtformats )
  target_link_libraries( bundle_aimsqtformats aimsqtformats )
  SOMA_INSTALL( TARGETS bundle_aimsqtformats
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )

  ADD_LIBRARY(bundle_aimsqtformats MODULE
  bundle/qtbundle.cc
  )
  
  TARGET_LINK_LIBRARIES(bundle_aimsqtformats
  aimsqtformats
  )
  
  SET_TARGET_PROPERTIES(bundle_aimsqtformats
  PROPERTIES OUTPUT_NAME aimsqtformats
  )
  
  INSTALL(TARGETS bundle_aimsqtformats DESTINATION lib/)

endif( soma-infra_FOUND )
