add_subdirectory(lib)
set(GRAPH_HEADERS ${GRAPH_HEADERS} PARENT_SCOPE)
#------------------------------------------------------------------------------
# Build targets for test commands by looking into "*_test" directories
#------------------------------------------------------------------------------
file( GLOB _commands "${CMAKE_CURRENT_SOURCE_DIR}/*_test" )
foreach( _command ${_commands} )
  file( GLOB _pro "${_command}/*.pro" )
  BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                              TARGET _target
                              SOURCES _proSources )
  set( _sources )
  foreach( _f ${_proSources} )
    set( _sources ${_sources} ${_command}/${_f} )
  endforeach( _f ${_proSources} )
  
  BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} 
                            HELP_GENERATE FALSE )
  target_link_libraries( ${_target} graph )
endforeach( _command ${commands} )
