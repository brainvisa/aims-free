add_library( bundle_aimsvida MODULE bundle/vidabundle.cc )
# set_property( TARGET bundle_aimsvida PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsvida PROPERTY OUTPUT_NAME aimsvida.${${PROJECT_NAME}_VERSION} )
target_link_libraries( bundle_aimsvida aimsvida )
BRAINVISA_INSTALL( TARGETS bundle_aimsvida
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
