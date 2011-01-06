add_library( bundle_aimsjpeg MODULE bundle/jpegbundle.cc )
# set_property( TARGET bundle_aimsjpeg PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsjpeg PROPERTY OUTPUT_NAME aimsjpeg.${${PROJECT_NAME}_VERSION} )
target_link_libraries( bundle_aimsjpeg aimsjpeg )
BRAINVISA_INSTALL( TARGETS bundle_aimsjpeg
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
