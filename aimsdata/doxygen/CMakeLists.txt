IF ( DOXYGEN_FOUND )
  SET( DOXYFILE_HTML_HEADER "\"${CMAKE_CURRENT_SOURCE_DIR}/head_aims.html\"" )
  set( _doxygenInput ${HEADERS} "config/config.h" )
  # remove some embedded external libs headers, some of which causing problems
  # to some doxygen versions (Ubuntu 13.10-14.04)
  list( REMOVE_ITEM _doxygenInput 
    "io/znzlib/znzlib.h"
    "io/niftilib/nifti1.h"
    "io/niftilib/nifti1_io.h"
    "io/gifticlib/gifti_io.h"
    "io/gifticlib/gifti_xml.h" )
  get_filename_component(cur_dir "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
  get_filename_component(component_name ${cur_dir} NAME)
  # generate docs in a "standard config", where blitz and kdtree are used.
  set( DOXYFILE_PREDEFINED
    ${AIMS-FREE_DEFINITIONS} ${SOMA-IO_DEFINITIONS} CARTO_USE_KDTREE
    CARTO_USE_BLITZ )
  set(aims_version
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set(_somaio_version "${soma-io_VERSION_MAJOR}.${soma-io_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES
    "${CMAKE_BINARY_DIR}/share/doc/graph-${aims_version}/doxygen/graph.tag=../../graph-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartobase-${_somaio_version}/doxygen/cartobase.tag=../../cartobase-${_somaio_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/soma-io-${_somaio_version}/doxygen/soma-io.tag=../../soma-io-${_somaio_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartodata-${aims_version}/doxygen/cartodata.tag=../../cartodata-${aims_version}/doxygen" )
  BRAINVISA_GENERATE_DOXYGEN_DOC(
    _doxygenInput head_aims.html aims.gif bvstyle.css
    INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/aims"
    COMPONENT "${component_name}" )
  add_dependencies(
    ${component_name}-doxygen
    graph-doxygen soma-io-doxygen cartodata-doxygen cartobase-doxygen )
ENDIF ( DOXYGEN_FOUND )
