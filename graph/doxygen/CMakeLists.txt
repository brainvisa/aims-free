IF ( DOXYGEN_FOUND )
#   SET( DOXYFILE_HTML_HEADER "\"${CMAKE_CURRENT_SOURCE_DIR}/head_graph.html\"" )
  set( _doxygenInput ${GRAPH_HEADERS} )
  get_filename_component(cur_dir "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
  get_filename_component(component_name ${cur_dir} NAME)
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen")
  set( DOXYFILE_PROJECT_LOGO "\"${CMAKE_CURRENT_SOURCE_DIR}/graph.gif\"" )
  set( DOXYFILE_PROJECT_BRIEF "\"Graph: generic attributed relational graphs\"" )
  BRAINVISA_GENERATE_DOXYGEN_DOC( _doxygenInput  head_graph.html graph.gif
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/graph"
                                  COMPONENT "${component_name}" )
  add_dependencies( ${component_name}-doxygen cartobase-doxygen )
ENDIF ( DOXYGEN_FOUND )
