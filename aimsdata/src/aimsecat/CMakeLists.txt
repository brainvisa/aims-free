SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/ecat.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsecat SHARED ${_sources} )
set_property( TARGET aimsecat PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsecat aimsdata ecat+ )
SOMA_INSTALL( TARGETS aimsecat
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
