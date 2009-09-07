BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers ecat+ headersTarget )
add_library( ecat+ SHARED ${_sources} )
add_dependencies( ecat+ ${headersTarget} )
set_property( TARGET ecat+ PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( ecat+ ecat )
BRAINVISA_INSTALL( TARGETS ecat+
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
BRAINVISA_GENERATE_DOXYGEN_DOC( _headers  )
