BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsalgo.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
# GET HEADERS FOR DOXYGEN DOC
set(AIMSALGO_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsalgo SHARED ${_sources} )
add_dependencies( aimsalgo ${headersTarget} )
set_property( TARGET aimsalgo PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsalgo aimsalgopub )
BRAINVISA_INSTALL( TARGETS aimsalgo
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
