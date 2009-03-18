add_library( bundle_aimstiff MODULE bundle/tiffbundle.cc )
set_property( TARGET bundle_aimstiff PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimstiff PROPERTY OUTPUT_NAME aimstiff )
target_link_libraries( bundle_aimstiff aimstiff )
SOMA_INSTALL( TARGETS bundle_aimstiff
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
