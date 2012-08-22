# BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}"
#                           bin
#                           ${PROJECT_NAME} )

# Add python commands as executables
file( GLOB _commands "${CMAKE_CURRENT_SOURCE_DIR}/*.py" )
foreach(_command ${_commands})
  get_filename_component( _command_path "${_command}" ABSOLUTE)
  get_filename_component( _command_name "${_command}" NAME)
  # message("Add pyaims command ${_command_name}, _command_path ${_command_path}")
  BRAINVISA_ADD_EXECUTABLE( ${_command_name} "${_command_path}" IS_SCRIPT )
endforeach()

unset(_commands)
unset(_command_path)
unset(_command_name)