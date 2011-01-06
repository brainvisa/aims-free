add_library( bundle_aimsqtformats MODULE bundle/qtbundle.cc )
set_property( TARGET bundle_aimsqtformats PROPERTY OUTPUT_NAME aimsqtformats.${${PROJECT_NAME}_VERSION} )
target_link_libraries( bundle_aimsqtformats aimsqtformats )
BRAINVISA_INSTALL( TARGETS bundle_aimsqtformats
              DESTINATION lib
              COMPONENT aims-gpl )
