BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
set( GRAPH_HEADERS ${_headers} PARENT_SCOPE )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers graph headersTarget )
add_library( graph SHARED ${_sources} )
add_dependencies( graph ${headersTarget} )
set_property( TARGET graph PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( graph cartobase )
if( USE_SOMA_IO )
  target_link_libraries( graph soma-io )
endif( USE_SOMA_IO )
BRAINVISA_INSTALL( TARGETS graph
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
