IF ( DOXYGEN_FOUND )
#  SET( DOXYFILE_HTML_HEADER "\"${CMAKE_CURRENT_SOURCE_DIR}/head_aimsalgo.html\"" )
  set( _doxygenInput ${HEADERS} )
  get_filename_component(cur_dir "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
  get_filename_component(component_name ${cur_dir} NAME)
  # generate docs in a "standard config", where blitz and kdtree are used.
  set( DOXYFILE_PREDEFINED
    ${AIMS-FREE_DEFINITIONS} ${SOMA-IO_DEFINITIONS} CARTO_USE_KDTREE )
  set(aims_version
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set(_somaio_version "${soma-io_VERSION_MAJOR}.${soma-io_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES
    "${CMAKE_BINARY_DIR}/share/doc/aimsdata-${aims_version}/doxygen/aimsdata.tag=../../aimsdata-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/graph-${aims_version}/doxygen/graph.tag=../../graph-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartobase-${_somaio_version}/doxygen/cartobase.tag=../../cartobase-${_somaio_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/soma-io-${_somaio_version}/doxygen/soma-io.tag=../../soma-io-${_somaio_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartodata-${aims_version}/doxygen/cartodata.tag=../../cartodata-${aims_version}/doxygen")
  set( DOXYFILE_PROJECT_LOGO "\"${CMAKE_CURRENT_SOURCE_DIR}/aims.gif\"" )
  set( DOXYFILE_PROJECT_BRIEF "\"Neuroimaging image processing\"" )
  BRAINVISA_GENERATE_DOXYGEN_DOC(
    _doxygenInput  head_aimsalgo.html aims.gif
    INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/aims"
    COMPONENT ${component_name} )
  add_dependencies( aimsalgo-doxygen
    aimsdata-doxygen cartodata-doxygen graph-doxygen soma-io-doxygen
    cartobase-doxygen )
ENDIF ( DOXYGEN_FOUND )
