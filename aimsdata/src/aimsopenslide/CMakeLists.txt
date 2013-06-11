BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/openslide.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSOPENSLIDE_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsopenslide SHARED ${_sources} )
add_dependencies( aimsopenslide ${headersTarget} )
set_property( TARGET aimsopenslide PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsopenslide aimsdata ${OPENSLIDE_LIBRARIES}  )
BRAINVISA_INSTALL( TARGETS aimsopenslide
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
