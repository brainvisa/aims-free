add_library( bundle_aimsdicom MODULE bundle/dicombundle.cc )
set_property( TARGET bundle_aimsdicom PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsdicom PROPERTY OUTPUT_NAME aimsdicom )
target_link_libraries( bundle_aimsdicom aimsdicom )
SOMA_INSTALL( TARGETS bundle_aimsdicom
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
