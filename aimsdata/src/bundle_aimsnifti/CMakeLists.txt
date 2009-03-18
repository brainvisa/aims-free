add_library( bundle_aimsnifti MODULE bundle/niftibundle.cc )
set_property( TARGET bundle_aimsnifti PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsnifti PROPERTY OUTPUT_NAME aimsnifti )
target_link_libraries( bundle_aimsnifti aimsnifti )
SOMA_INSTALL( TARGETS bundle_aimsnifti
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
