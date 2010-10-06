BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/ecat.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSECAT_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsecat SHARED ${_sources} )
add_dependencies( aimsecat ${headersTarget} )
set_property( TARGET aimsecat PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsecat aimsdata ecat+ )
BRAINVISA_INSTALL( TARGETS aimsecat
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
