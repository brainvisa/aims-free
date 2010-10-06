IF ( DOXYGEN_FOUND )
  SET( DOXYFILE_HTML_HEADER "\"${CMAKE_CURRENT_SOURCE_DIR}/head_aims.html\"" )
  set( _doxygenInput ${HEADERS} )
  get_filename_component(cur_dir "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
  get_filename_component(component_name ${cur_dir} NAME)
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/graph-${aims_version}/doxygen/graph.tag=../../graph-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartodata-${aims_version}/doxygen/cartodata.tag=../../cartodata-${aims_version}/doxygen")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _doxygenInput head_aims.html aims.gif
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/aims"
                                  COMPONENT "${component_name}" )
ENDIF ( DOXYGEN_FOUND )
