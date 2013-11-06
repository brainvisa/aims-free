BRAINVISA_GET_FILE_LIST_FROM_PRO( ${CMAKE_CURRENT_SOURCE_DIR}/library.pro 
                                  "HEADERS" _headers 
                                  "SOURCES" _sources
                                )

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
add_library( cartodata SHARED ${_sources} )

add_dependencies( cartodata ${headersTarget} )
set_property( TARGET cartodata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartodata ${SOMA-IO_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS cartodata
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )

if( DOXYGEN_FOUND )
  set( component_name "cartodata" )
  # generate docs in a "standard config", where blitz is used.
  set( DOXYFILE_PREDEFINED
    ${AIMS_DEFINITIONS} ${SOMA-IO_DEFINITIONS} CARTO_USE_BLITZ )
  set( aims_version
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
  set(_somaio_version "${soma-io_VERSION_MAJOR}.${soma-io_VERSION_MINOR}")
  set( _doxyheaders ${_headers} "config/config.h" ) # add config.h file
  set( DOXYFILE_TAGFILES
    "${CMAKE_BINARY_DIR}/share/doc/cartobase-${_somaio_version}/doxygen/cartobase.tag=../../cartobase-${_somaio_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/soma-io-${_somaio_version}/doxygen/soma-io.tag=../../soma-io-${_somaio_version}/doxygen" )
  BRAINVISA_GENERATE_DOXYGEN_DOC(
    _doxyheaders
    INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}" 
    COMPONENT "${component_name}")
  add_dependencies( ${component_name}-doxygen soma-io-doxygen
    cartobase-doxygen )
endif( DOXYGEN_FOUND )
