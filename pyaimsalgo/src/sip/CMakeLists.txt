BRAINVISA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/sip.pro" "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers pyaimsalgo headersTarget )

file( GLOB _sipTemplates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
BRAINVISA_COPY_FILES( ${PROJECT_NAME}-dev
                      aimsalgotypessub.py ${_sipTemplates}
                      SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                      DESTINATION "${AIMS_RELATIVE_SIP_DIRECTORY}"
                      IMMEDIATE )

file( GLOB _sipSources "${CMAKE_CURRENT_SOURCE_DIR}/*.sip" )

if( SIP_MODULE )
  set( extra_defs "-D" "SIP_MODULE=\"${SIP_MODULE}\"" )
endif()


execute_process( COMMAND "${PYTHON_HOST_EXECUTABLE}"
        "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}/maketemplates.py"
        -i "${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py"
        -p "${CMAKE_CURRENT_SOURCE_DIR}"
        -o "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
        -t "${PYAIMS_SOURCE_DIR}/src/sip/typessub.py"
        -t "${CMAKE_CURRENT_SOURCE_DIR}/aimsalgotypessub.py"
        -l
        -m "${QT_MOC_EXECUTABLE}"
        -P "${CMAKE_C_PREPROCESSOR}"
        ${extra_defs}
  OUTPUT_VARIABLE _generatedSipFileList 
  RESULT_VARIABLE _returnValue )

if( NOT _returnValue EQUAL 0 )
  message( SEND_ERROR "The sip generation from pyaimsalgo templates with \"${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}/maketemplates.py\" returned an error:\n${_generatedSipFileList}")
endif( NOT _returnValue EQUAL 0 )

string( STRIP "${_generatedSipFileList}" _generatedSipFileList )
BRAINVISA_ADD_SIP_PYTHON_MODULE( aimsalgosip
  soma/aimsalgo
  "aimsalgo.sip"
  SIP_SOURCES ${_generatedSipFileList} ${_sipSources}
  SIP_INCLUDE "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
    "${PYAIMS_SOURCE_DIR}/src/sip"
  SIP_INSTALL "${AIMS_RELATIVE_SIP_DIRECTORY}" )
target_link_libraries( aimsalgosip "${PYTHON_LIBRARY}" cartobase soma-io cartodata aimsdata aimsalgo pyaims )
add_dependencies( aimsalgosip ${headersTarget} )

BRAINVISA_INSTALL( TARGETS aimsalgosip 
                   DESTINATION ${PYTHON_INSTALL_DIRECTORY}/soma/aimsalgo
                   COMPONENT ${PROJECT_NAME} )
