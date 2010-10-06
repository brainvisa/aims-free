BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsply.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSPLY_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsply SHARED ${_sources} )
add_dependencies(  aimsply ${headersTarget} )
set_property( TARGET aimsply PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsply aimsdata )
BRAINVISA_INSTALL( TARGETS aimsply
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
