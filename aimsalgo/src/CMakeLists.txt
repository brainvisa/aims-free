add_subdirectory( public )
add_subdirectory( aimsalgo )
set(HEADERS ${AIMSALGO_HEADERS} ${PUBLIC_HEADERS} PARENT_SCOPE)
#------------------------------------------------------------------------------
# Build targets for commands by looking into directories listed in src.pro
#------------------------------------------------------------------------------
BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/src.pro "PSUBDIRS" _commands )
foreach( _command ${_commands} )
  file( GLOB _pro "${_command}/*.pro" )
  BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                              TARGET _target
                              SOURCES _proSources )
  set( _sources )
  foreach( _f ${_proSources} )
    set( _sources ${_sources} ${_command}/${_f} )
  endforeach( _f ${_proSources} )
  BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
  target_link_libraries( ${_target} aimsalgo )
  BRAINVISA_INSTALL( TARGETS ${_target}
                DESTINATION bin
                COMPONENT ${PROJECT_NAME} )
endforeach( _command ${commands} )

# Targets linking with aimsecat
foreach( _command AimsFovSplit AimsSumPETFrame )
  file( GLOB _pro "${_command}/*.pro" )
  BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                              TARGET _target
                              SOURCES _proSources )
  set( _sources )
  foreach( _f ${_proSources} )
    set( _sources ${_sources} ${_command}/${_f} )
  endforeach( _f ${_proSources} )
  BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
  target_link_libraries( ${_target} aimsalgo aimsecat )
  BRAINVISA_INSTALL( TARGETS ${_target}
                DESTINATION bin
                COMPONENT ${PROJECT_NAME} )
endforeach( _command AimsFovSplit AimsSumPETFrame )
  
# Targets linking with aimsjpeg
if( JPEG_FOUND )
  foreach( _command AimsData2Jpeg )
    file( GLOB _pro "${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach( _f ${_proSources} )
    BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsalgo aimsjpeg )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endforeach( _command AimsData2Jpeg )
endif( JPEG_FOUND )
# Targets linking with aimsdicom
IF( DCMTK_FOUND AND JASPER_FOUND AND TIFF_FOUND AND PNG12_FOUND )
  include_directories( ${DCMTK_INCLUDE_DIR} )
  foreach( _command AimsSiemensEPI2Volume )
    file( GLOB _pro "${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach( _f ${_proSources} )
    BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsalgo aimsdicom )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endforeach( _command AimsData2Jpeg )
endif( DCMTK_FOUND )
