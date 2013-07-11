BRAINVISA_GET_FILE_LIST_FROM_PRO( ${CMAKE_CURRENT_SOURCE_DIR}/library.pro 
                                  "HEADERS" _headers 
                                  "HEADERS_NO_USE_SOMA_IO" _headers_no_use 
                                  "HEADERS_USE_SOMA_IO" _headers_use
                                  "SOURCES" _sources
                                  "SOURCES_NO_USE_SOMA_IO" _sources_no_use 
                                  "SOURCES_USE_SOMA_IO" _sources_use 
                                )

if( USE_SOMA_IO )
  set( _headers "${_headers}" "${_headers_use}" )
  set( _sources "${_sources}" "${_sources_use}" )
else()
  set( _headers "${_headers}" "${_headers_no_use}" )
  set( _sources "${_sources}" "${_sources_no_use}" )
endif()
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
add_library( cartodata SHARED ${_sources} )

add_dependencies( cartodata ${headersTarget} )
set_property( TARGET cartodata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartodata cartobase )
if( USE_SOMA_IO )
  target_link_libraries( cartodata soma-io )
endif( USE_SOMA_IO )
BRAINVISA_INSTALL( TARGETS cartodata
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )

if( DOXYGEN_FOUND )
  set( component_name "cartodata" )
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}" )
  set( aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen" )
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}" 
                                  COMPONENT "${component_name}")
  add_dependencies( ${component_name}-doxygen cartobase-doxygen )
endif( DOXYGEN_FOUND )
