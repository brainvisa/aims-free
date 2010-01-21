BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/bundleecat.pro "HEADERS" _headers "SOURCES" _sources )
add_library( bundle_aimsecat MODULE ${_sources} )
# set_property( TARGET bundle_aimsecat.${${PROJECT_NAME}_VERSION} PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsecat PROPERTY OUTPUT_NAME aimsecat.${${PROJECT_NAME}_VERSION} )
target_link_libraries( bundle_aimsecat aimsecat )
BRAINVISA_INSTALL( TARGETS bundle_aimsecat
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
