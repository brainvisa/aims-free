SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers ecat ${PROJECT_NAME}-devel )
add_library( ecat SHARED ${_sources} )
set_property( TARGET ecat PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
SOMA_INSTALL( TARGETS ecat
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
SOMA_GENERATE_DOXYGEN_DOC( ecat-doxygen _headers  )
