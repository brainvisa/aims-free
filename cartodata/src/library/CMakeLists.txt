set( _headers
     doc/cartodata_doc.h
     io/carto2aimsheadertranslator.h
     io/volumeformatreader.h
     io/volumeformatreader_d.h
     io/volumeformatwriter.h
     io/volumeformatwriter_d.h
     io/volumeutilio.h
     io/volumeutilio_d.h
     volume/functional.h
     volume/volume.h
     volume/volume_d.h
     volume/volumebase.h
     volume/volumebase_d.h
     volume/volumebase_d_inline.h
     volume/volumebase_d_operators.h
     volume/volumebase_d_instantiate.h
     volume/volumeref.h
     volume/volumeref_d.h
     volume/volumeref_d_inline.h
     volume/volumeref_d_operators.h
     volume/volumeref_d_instantiate.h
     volume/volumeoperators.h
     volume/volumeproxy.h
     volume/volumeproxy_d.h
     volume/volumeutil.h
     volume/volumeutil_d.h
     volume/volumeview.h
     wip/roi/siteIterator.h
     wip/roi/roi.h
     wip/roi/doi.h
)

set ( _sources
      config/plugins.cc
      io/carto2aimsheadertranslator.cc
      io/somaio_declare.cc
      io/volumeutilio.cc
      volume/volume.cc
      volume/volume_vector.cc
      wip/roi/siteIterator.cc
      wip/roi/roi.cc
      wip/roi/doi.cc
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
    ${AIMS_DEFINITIONS} ${SOMA-IO_DEFINITIONS} )
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
