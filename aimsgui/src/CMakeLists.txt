add_subdirectory( lib )
add_subdirectory( aimsqtformats )
add_subdirectory( aimsqsqlgraphformat )
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
