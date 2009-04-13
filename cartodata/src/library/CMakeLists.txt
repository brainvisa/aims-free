BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/library.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata )
add_library( cartodata SHARED ${_sources} )
set_property( TARGET cartodata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartodata cartobase )
BRAINVISA_INSTALL( TARGETS cartodata
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
BRAINVISA_GENERATE_DOXYGEN_DOC( _headers  )
