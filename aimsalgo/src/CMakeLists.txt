add_subdirectory( aimsalgo )
set(HEADERS ${AIMSALGO_HEADERS} ${PUBLIC_HEADERS} PARENT_SCOPE)

# don't warn on obsolete classes within this library: we still provide obsolete
# features so we have to build them without warnings
add_definitions( "-DAIMSDATA_CLASS_NO_DEPREC_WARNING=1" )

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
  endforeach()
  BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
  target_link_libraries( ${_target} aimsalgo )
  BRAINVISA_INSTALL( TARGETS ${_target}
                DESTINATION bin
                COMPONENT ${PROJECT_NAME} )
endforeach()

add_subdirectory( AimsBundleSelectCenter )
# add_subdirectory( AimsMeshCleaner )


# Targets linking with aimsecat
if( BUILD_ECAT )
  foreach( _command AimsFovSplit AimsSumPETFrame )
    file( GLOB _pro "${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach()
    BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsalgo aimsecat )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endforeach()
endif( BUILD_ECAT )
  
# Targets linking with aimsdicom.
if( DCMTK_FOUND )
  include_directories( ${DCMTK_INCLUDE_DIR} )
  foreach( _command AimsSiemensEPI2Volume )
    file( GLOB _pro "${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach()

    if ( NOT WIN32 )
      add_definitions( -DHAVE_CONFIG_H )
# Not necessary anymore (probably for version >= 3.6.0)
# it generates warning during compilation
#   else ()
#     add_definitions( -DUSE_STD_CXX_INCLUDES )
    endif ()

    BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsalgo )

    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endforeach()
endif()

# Targets linking with gfortran
if( CMAKE_Fortran_COMPILER )
  foreach( _command AimsRegisterFFDEstimate )
    file( GLOB _pro "${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach()
    BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsalgo gfortran )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endforeach()
endif()

#add_subdirectory( tests )
