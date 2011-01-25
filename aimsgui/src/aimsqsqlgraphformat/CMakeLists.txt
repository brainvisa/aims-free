BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/qsqlgraphformat.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSQSQLGRAPHFORMAT_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsqsqlgraphformat SHARED ${_sources} )
add_dependencies( aimsqsqlgraphformat ${headersTarget} )
set_property( TARGET aimsqsqlgraphformat PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsqsqlgraphformat aimsgui )
BRAINVISA_INSTALL( TARGETS aimsqsqlgraphformat
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
