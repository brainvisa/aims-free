BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers vidaIO headersTarget )
add_library( vidaIO SHARED ${_sources} )
add_dependencies( vidaIO ${headersTarget} )
set_property( TARGET vidaIO PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( vidaIO m )
BRAINVISA_INSTALL( TARGETS vidaIO
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
BRAINVISA_GENERATE_DOXYGEN_DOC( _headers  )
