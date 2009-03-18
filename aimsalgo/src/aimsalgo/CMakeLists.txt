SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsalgo.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims ${PROJECT_NAME}-devel )
add_library( aimsalgo SHARED ${_sources} )
set_property( TARGET aimsalgo PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsalgo aimsalgopub )
SOMA_INSTALL( TARGETS aimsalgo
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
