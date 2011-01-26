BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers ecat+ headersTarget )
add_library( ecat+ SHARED ${_sources} )
add_dependencies( ecat+ ${headersTarget} )
set_property( TARGET ecat+ PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( ecat+ ecat )
BRAINVISA_INSTALL( TARGETS ecat+
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )

IF ( DOXYGEN_FOUND )
  set(component_name "ecat+")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/ecat-${aims_version}/doxygen/ecat.tag=../../ecat-${aims_version}/doxygen")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}" 
                                  COMPONENT "${component_name}")
  add_dependencies( ${component_name}-doxygen ecat-doxygen )
ENDIF ( DOXYGEN_FOUND )
