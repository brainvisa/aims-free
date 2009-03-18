add_library( bundle_aimsminc MODULE bundle/mincbundle.cc )
set_property( TARGET bundle_aimsminc PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsminc PROPERTY OUTPUT_NAME aimsminc )
target_link_libraries( bundle_aimsminc aimsminc )
SOMA_INSTALL( TARGETS bundle_aimsminc
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
