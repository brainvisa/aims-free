if( soma-infra_FOUND )
  
  SOMA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/sip.pro" "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers pyaimsalgo aims-devel )
  
  file( GLOB _sipTemplates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
  SOMA_COPY_FILES( aims-pyaimsalgotemplates aims-devel
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
    soma/aims aims aims-devel 
    "aimsalgo.sip"
    SIP_SOURCES ${_generatedSipFileList} ${_sipSources}
    SIP_INCLUDE "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}"
      "${PYAIMS_SOURCE_DIR}/src/sip" )
  target_link_libraries( aimsalgosip ${PYTHON_LIBRARY} cartobase cartodata aimsdata aimsalgo pyaims )

  SOMA_INSTALL( TARGETS aimsalgosip 
                DESTINATION python/soma/aims
                COMPONENT aims )

else( soma-infra_FOUND )

  getFromPro(sip.pro SIP_HDRS SIP_SRCS)
  
  SHFJ_COPY_HEADERS(include/pyaimsalgo ${SIP_HDRS})
  
  FILE(GLOB globbedTpl           ${CMAKE_CURRENT_SOURCE_DIR}/*.tpl)
  FILE(GLOB globbedSip           ${CMAKE_CURRENT_SOURCE_DIR}/*.sip)
  FILE(GLOB relativeTpl RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.tpl)
  FILE(GLOB relativeSip RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.sip)
  
  
  # Copy sip & tpl files for further compilation
  COPY_FILES(${CMAKE_BINARY_DIR}/sip ${globbedSip})
  COPY_FILES(${CMAKE_BINARY_DIR}/tpl ${globbedTpl})
  
  
  SET(SIP_TYPESSUB                  ${CMAKE_CURRENT_SOURCE_DIR}/aimsalgotypessub.py)
  SET(makeTemplatesGeneratedTypePy  ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py)
  SET(TPL_DIRECTORY                 ${CMAKE_BINARY_DIR}/tpl)
  
  
  SIP_MAKE_TEMPLATES(${makeTemplatesGeneratedTypePy} ${TPL_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/sip ${globbedTpl})
  
  
  # Get the .sip files that have been created
  FILE(GLOB SIPPED_TPL ${CMAKE_CURRENT_BINARY_DIR}/sip/*.sip)
  
  
  # copy the sipped files to a common directory so that external libraries
  # can access them for further compilation
  COPY_FILES(${CMAKE_BINARY_DIR}/sip ${SIPPED_TPL})
  
  
  # This is real confusing: we have set at first the path to aims.sip, otherwise some
  # symbols are reported as missing: we cannot copy all sip files into a common
  # directory but rather use the source directory.
  
  SET(SIP_INCLUDES
  ${PYAIMS_SIP_DIR}  # defined by pyaims
  ${CMAKE_BINARY_DIR}/sip
  ${PYQT_SIP_DIR}
  )
  
  SET(SIP_CONCAT_PARTS 1)
  SET(SIP_TAGS ALL WS_X11 Qt_3_3_0)
  SET(SIP_DISABLE_FEATURES 
  VendorID 
  Qt_STYLE_WINDOWSXP 
  Qt_STYLE_INTERLACE
  Qt_STYLE_WINDOWSXP
  #CARTODATA # Should we keep it?
  )
  SET(SIP_EXTRA_OPTIONS -e)
  SET(SIP_EXTRA_DEPENDENCIES ${SIPPED_TPL} ${globbedSip})
  
  SET(SIP_MAIN_FILE aimsalgo.sip)
  
  
  ADD_SIP_PYTHON_MODULE(aimsalgosip ${SIP_MAIN_FILE}
  aimsalgo
  aimsdata
  cartodata
  cartobase
  )
  
  INSTALL(TARGETS aimsalgosip DESTINATION python/soma/aimsalgo)

endif( soma-infra_FOUND )
