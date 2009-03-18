if( AIMS_PARTIAL_AIMSSIP )
  set( _mainSipFile aimsLight.sip )
  set( _generatedtypes_py ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypesLight.py )
else( AIMS_PARTIAL_AIMSSIP )
  set( _mainSipFile aims.sip )
  set( _generatedtypes_py ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py )
endif( AIMS_PARTIAL_AIMSSIP )

SOMA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/sip.pro" "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers pyaims ${PROJECT_NAME}-devel )

file( GLOB _sipTemplates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
SOMA_COPY_FILES( aims-pyaimstemplates ${PROJECT_NAME}-devel maketemplates.py maketemplate.py typessub.py ${_sipTemplates}
                  SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  DESTINATION "${AIMS_RELATIVE_SIP_DIRECTORY}" )

file( GLOB _sipSources "${CMAKE_CURRENT_SOURCE_DIR}/*.sip" )

file( MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}" )
exec_program( ${PYTHON_EXECUTABLE}
  ARGS "${PYAIMS_SOURCE_DIR}/src/sip/maketemplates.py"
        -i "${_generatedtypes_py}"
        -p "${CMAKE_CURRENT_SOURCE_DIR}"
        -o "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
        -t "${CMAKE_CURRENT_SOURCE_DIR}/typessub.py"
        -l
  OUTPUT_VARIABLE _generatedSipFileList 
  RETURN_VALUE _returnValue )

if( NOT _returnValue EQUAL 0 )
  message( FATAL_ERROR "The sip generation from pyaims templates returned an error:\n  ${_generatedSipFileList}\n========================")
endif( NOT _returnValue EQUAL 0 )

SOMA_ADD_SIP_PYTHON_MODULE( aimssip 
  soma/aims ${PROJECT_NAME} ${PROJECT_NAME}-devel
  "${_mainSipFile}" 
  SIP_SOURCES ${_generatedSipFileList} ${_sipSources}
  SIP_INCLUDE  "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}" )
target_link_libraries( aimssip ${PYTHON_LIBRARY} cartobase cartodata graph aimsdata pyaims )

SOMA_INSTALL( TARGETS aimssip 
              DESTINATION python/soma/aims
              COMPONENT ${PROJECT_NAME} )

