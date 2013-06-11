BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/openslide.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTOOPENSLIDE_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartoopenslide SHARED ${_sources} )
add_dependencies( cartoopenslide ${headersTarget} )
set_property( TARGET cartoopenslide PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartoopenslide cartodata ${OPENSLIDE_LIBRARIES}  )
BRAINVISA_INSTALL( TARGETS cartoopenslide
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
