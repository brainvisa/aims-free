SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsply.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsply SHARED ${_sources} )
set_property( TARGET aimsply PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsply aimsdata )
SOMA_INSTALL( TARGETS aimsply
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
