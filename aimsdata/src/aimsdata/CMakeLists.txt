SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsdata.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims ${PROJECT_NAME}-devel )
add_library( aimsdata SHARED ${_sources} )
set_property( TARGET aimsdata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsdata graph cartodata )
SOMA_INSTALL( TARGETS aimsdata
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
