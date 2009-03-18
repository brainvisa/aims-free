SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers graph graph-devel )
add_library( graph SHARED ${_sources} )
set_property( TARGET graph PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( graph cartobase )
SOMA_INSTALL( TARGETS graph
              DESTINATION lib
              COMPONENT graph )
SOMA_GENERATE_DOXYGEN_DOC( graph-doxygen _headers  )
