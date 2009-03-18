SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/vida.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsvida SHARED ${_sources} )
set_property( TARGET aimsvida PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsvida aimsdata vidaIO )
SOMA_INSTALL( TARGETS aimsvida
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
