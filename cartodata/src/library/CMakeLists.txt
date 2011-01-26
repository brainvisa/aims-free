BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/library.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
add_library( cartodata SHARED ${_sources} )
add_dependencies( cartodata ${headersTarget} )
set_property( TARGET cartodata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartodata cartobase )
BRAINVISA_INSTALL( TARGETS cartodata
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
              
IF ( DOXYGEN_FOUND )
  SET(component_name "cartodata")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}" 
                                  COMPONENT "${component_name}")
  add_dependencies( ${component_name}-doxygen cartobase-doxygen )
ENDIF ( DOXYGEN_FOUND )
