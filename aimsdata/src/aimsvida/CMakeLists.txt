BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/vida.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSVIDA_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsvida SHARED ${_sources} )
add_dependencies( aimsvida ${headersTarget} )
set_property( TARGET aimsvida PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsvida aimsdata vidaIO )
BRAINVISA_INSTALL( TARGETS aimsvida
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
