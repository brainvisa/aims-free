add_subdirectory( lib )
set(HEADERS ${AIMSGUI_HEADERS})
add_subdirectory( aimsqtformats )
set(HEADERS ${AIMSQTFORMATS_HEADERS} ${HEADERS})
add_subdirectory( aimsqsqlgraphformat )
set(HEADERS ${AIMSQSQLGRAPHFORMAT_HEADERS} ${HEADERS})
if( APPLE )
  add_subdirectory( bundle_aimsqtformats )
endif( APPLE )

#------------------------------------------------------------------------------
# Build targets for commands by looking into "Aims*" directories
#------------------------------------------------------------------------------
foreach( _command AimsAttributedViewer AimsLabelSelector )
  if( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_command}" )
    file( GLOB _pro "${CMAKE_CURRENT_SOURCE_DIR}/${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach()

    add_executable( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsgui )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endif( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_command}" )
endforeach( _command )



IF ( DOXYGEN_FOUND )
  set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/graph-${aims_version}/doxygen/graph.tag=../../graph-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartodata-${aims_version}/doxygen/cartodata.tag=../../cartodata-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/aimsdata-${aims_version}/doxygen/aimsdata.tag=../../aimsdata-${aims_version}/doxygen")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  BRAINVISA_GENERATE_DOXYGEN_DOC( HEADERS 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/aims" 
                                  COMPONENT aimsgui )
  add_dependencies( aimsgui-doxygen graph-doxygen cartobase-doxygen cartodata-doxygen aimsdata-doxygen )
ENDIF ( DOXYGEN_FOUND )
