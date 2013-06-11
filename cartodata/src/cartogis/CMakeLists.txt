BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/gis.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTOGIS_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartogis SHARED ${_sources} )
add_dependencies( cartogis ${headersTarget} )
set_property( TARGET cartogis PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartogis cartodata )
BRAINVISA_INSTALL( TARGETS cartogis
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
