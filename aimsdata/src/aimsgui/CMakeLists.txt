if( soma-infra_FOUND )

  add_definitions( ${QT_DEFINITIONS} )
  include_directories( ${QT_INCLUDE_DIR} )

  add_subdirectory( lib )

  #------------------------------------------------------------------------------
  # Build targets for commands by looking into "Aims*" directories
  #------------------------------------------------------------------------------
  file( GLOB _commands "${CMAKE_CURRENT_SOURCE_DIR}/Aims*" )
  foreach( _command ${_commands} )
    if( IS_DIRECTORY "${_command}" )
    file( GLOB _pro "${_command}/*.pro" )
    SOMA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                HEADERS _proHeaders
                                SOURCES _proSources )
    set( _headers )
    foreach( _f ${_proHeaders} )
      set( _headers ${_headers} ${_command}/${_f} )
    endforeach( _f ${_proHeaders} )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach( _f ${_proSources} )
    SOMA_ADD_MOC_FILES( _sources ${_headers} )
    add_executable( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsgui )
    SOMA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT aimsdata )
    endif( IS_DIRECTORY "${_command}" )
   endforeach( _command ${commands} )

else( soma-infra_FOUND )

  add_subdirectory( lib )
  add_subdirectory( AimsLabelSelector )
  add_subdirectory( AimsAttributedViewer )

endif( soma-infra_FOUND )
