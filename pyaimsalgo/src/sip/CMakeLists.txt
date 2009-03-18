SOMA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/sip.pro" "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers pyaimsalgo ${PROJECT_NAME}-devel )

file( GLOB _sipTemplates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
SOMA_COPY_FILES( aims-pyaimsalgotemplates ${PROJECT_NAME}-devel
                  aimsalgotypessub.py ${_sipTemplates}
                  SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  DESTINATION "${AIMS_RELATIVE_SIP_DIRECTORY}" )

file( GLOB _sipSources "${CMAKE_CURRENT_SOURCE_DIR}/*.sip" )

exec_program( ${PYTHON_EXECUTABLE}
  ARGS "${PYAIMS_SOURCE_DIR}/src/sip/maketemplates.py"
        -i "${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py"
        -p "${CMAKE_CURRENT_SOURCE_DIR}"
        -o "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
        -t "${PYAIMS_SOURCE_DIR}/src/sip/typessub.py"
        -t "${CMAKE_CURRENT_SOURCE_DIR}/aimsalgotypessub.py"
        -l
  OUTPUT_VARIABLE _generatedSipFileList 
  RETURN_VALUE _returnValue )

if( NOT _returnValue EQUAL 0 )
  message( FATAL_ERROR "The sip generation from pyaimsalgo templates returned an error:\n  ${_generatedSipFileList}")
endif( NOT _returnValue EQUAL 0 )

SOMA_ADD_SIP_PYTHON_MODULE( aimsalgosip
  soma/aims ${PROJECT_NAME} ${PROJECT_NAME}-devel 
  "aimsalgo.sip"
  SIP_SOURCES ${_generatedSipFileList} ${_sipSources}
  SIP_INCLUDE "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
    "${PYAIMS_SOURCE_DIR}/src/sip" )
target_link_libraries( aimsalgosip ${PYTHON_LIBRARY} cartobase cartodata aimsdata aimsalgo pyaims )

SOMA_INSTALL( TARGETS aimsalgosip 
              DESTINATION python/soma/aims
              COMPONENT ${PROJECT_NAME} )
