if( AIMS_PARTIAL_AIMSSIP )
  set( _mainSipFile "${CMAKE_CURRENT_SOURCE_DIR}/aimsLight.sip" )
  set( _generatedtypes_py "${CMAKE_CURRENT_SOURCE_DIR}/generatedtypesLight.py" )
else( AIMS_PARTIAL_AIMSSIP )
  set( _mainSipFile "${CMAKE_CURRENT_SOURCE_DIR}/aims.sip" )
  set( _generatedtypes_py "${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py" )
endif( AIMS_PARTIAL_AIMSSIP )

BRAINVISA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/sip.pro" "HEADERS" _headers "SOURCES" _sources )
SET(PYAIMSSIP_HEADERS ${_headers} PARENT_SCOPE)
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers pyaims headersTarget )

file( GLOB _sipTemplates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
BRAINVISA_COPY_FILES( ${PROJECT_NAME}-dev maketemplates.py maketemplate.py typessub.py ${_sipTemplates}
                      SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                      DESTINATION "${AIMS_RELATIVE_SIP_DIRECTORY}" 
                      IMMEDIATE )

file( GLOB _sipSources "${CMAKE_CURRENT_SOURCE_DIR}/*.sip" )

file( MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}" )

if( SIP_MODULE )
  set( extra_defs "-D" "SIP_MODULE=\"${SIP_MODULE}\"" )
endif()
if( SIP_ABI )
  string( REGEX MATCH "[^.]*" SIP_ABI_MAJOR_VERSION "${SIP_ABI}")
  set( extra_defs "${extra_defs}" "-D"
       "SIP_ABI_MAJOR_VERSION=${SIP_ABI_MAJOR_VERSION}")
endif()

if( QT_MOC_EXECUTABLE )
  set( MT_MOC_OPTS "-m" "${QT_MOC_EXECUTABLE}" )
endif()

set(__command "${PYTHON_HOST_EXECUTABLE}"
        "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}/maketemplates.py"
        -i "${_generatedtypes_py}"
        -p "${CMAKE_CURRENT_SOURCE_DIR}"
        -o "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
        -t "${CMAKE_CURRENT_SOURCE_DIR}/typessub.py"
        -l
        ${MT_MOC_OPTS}
        -P "${CMAKE_C_PREPROCESSOR}"
        ${extra_defs} )
if(CMAKE_CROSSCOMPILING AND WIN64)
    list(APPEND __command -T "windows-64")
endif()
message("RUN MAKETEMPLATES: ${__command}")
execute_process( COMMAND ${__command}
  OUTPUT_VARIABLE _generatedSipFileList 
  RESULT_VARIABLE _returnValue )
string( STRIP "${_generatedSipFileList}" _generatedSipFileList )
# message("output: ${_generatedSipFileList}")

if( NOT _returnValue EQUAL 0 )
  message( SEND_ERROR "The sip generation from pyaims templates failed. Command:\n${_command}\nreturned output:\n${_generatedSipFileList}\n========================")
# else( NOT _returnValue EQUAL 0 )
#   message("Command:\n${_command}\ngenerated sip files:\n${_generatedSipFileList}
#   \n========================")
endif( NOT _returnValue EQUAL 0 )
unset(__command)

BRAINVISA_ADD_SIP_PYTHON_MODULE( aimssip 
  soma/aims
  "${_mainSipFile}" 
  SIP_SOURCES ${_generatedSipFileList} ${_sipSources}
  SIP_INCLUDE "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
  SIP_INSTALL "${AIMS_RELATIVE_SIP_DIRECTORY}"
  NUM_OUTPUT_FILES 16 )
target_link_libraries( aimssip "${PYTHON_LIBRARY}" cartobase soma-io cartodata graph aimsdata pyaims )

add_dependencies( aimssip ${headersTarget} )

BRAINVISA_INSTALL( TARGETS aimssip 
                   DESTINATION ${PYTHON_INSTALL_DIRECTORY}/soma/aims
                   COMPONENT ${PROJECT_NAME} )

