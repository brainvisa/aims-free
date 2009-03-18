SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers vidaIO ${PROJECT_NAME}-devel )
add_library( vidaIO SHARED ${_sources} )
set_property( TARGET vidaIO PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( vidaIO m )
SOMA_INSTALL( TARGETS vidaIO
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
SOMA_GENERATE_DOXYGEN_DOC( vidaIO-doxygen _headers  )
