if( soma-infra_FOUND )

  add_subdirectory(lib)
  
  #------------------------------------------------------------------------------
  # Build targets for test commands by looking into "*_test" directories
  #------------------------------------------------------------------------------
  file( GLOB _commands "${CMAKE_CURRENT_SOURCE_DIR}/*_test" )
  foreach( _command ${_commands} )
    file( GLOB _pro "${_command}/*.pro" )
    SOMA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach( _f ${_proSources} )
    add_executable( ${_target} ${_sources} )
    target_link_libraries( ${_target} graph )
  endforeach( _command ${commands} )

else( soma-infra_FOUND )

  ADD_SUBDIRECTORY(lib)
  
  ADD_SUBDIRECTORY(counter_test)
  ADD_SUBDIRECTORY(extract_test)
  ADD_SUBDIRECTORY(gio_test)
  ADD_SUBDIRECTORY(graph_test)
  ADD_SUBDIRECTORY(syntax_test)
  ADD_SUBDIRECTORY(tio_test)

endif( soma-infra_FOUND )
