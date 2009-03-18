SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/bundleply.pro "HEADERS" _headers "SOURCES" _sources )
add_library( bundle_aimsply MODULE ${_sources} )
set_property( TARGET bundle_aimsply PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
set_property( TARGET bundle_aimsply PROPERTY OUTPUT_NAME aimsply )
target_link_libraries( bundle_aimsply aimsply )
SOMA_INSTALL( TARGETS bundle_aimsply
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
