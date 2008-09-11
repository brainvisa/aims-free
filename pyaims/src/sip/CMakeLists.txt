if( soma-infra_FOUND )

  if( AIMS_PARTIAL_AIMSSIP )
    set( _mainSipFile aimsLight.sip )
    set( _generatedtypes_py ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypesLight.py )
  else( AIMS_PARTIAL_AIMSSIP )
    set( _mainSipFile aims.sip )
    set( _generatedtypes_py ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py )
  endif( AIMS_PARTIAL_AIMSSIP )
  
  SOMA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/sip.pro" "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers pyaims aims-devel )
  
  file( GLOB _sipTemplates RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.tpl" )
  SOMA_COPY_FILES( aims-pyaimstemplates aims-devel maketemplates.py maketemplate.py typessub.py ${_sipTemplates}
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
    soma/aims aims aims-devel
    "${_mainSipFile}" 
    SIP_SOURCES ${_generatedSipFileList} ${_sipSources}
    SIP_INCLUDE  "${CMAKE_BINARY_DIR}/${AIMS_RELATIVE_SIP_DIRECTORY}" )
  target_link_libraries( aimssip ${PYTHON_LIBRARY} cartobase cartodata graph aimsdata pyaims )

  SOMA_INSTALL( TARGETS aimssip 
                DESTINATION python/soma/aims
                COMPONENT aims )

else( soma-infra_FOUND )

  OPTION(Use_partial_aimssip "Use only parts of PyAims, shorter to compile, for devel purpose" OFF)
  
  IF (Use_partial_aimssip)
    SET (SIP_MAIN_FILE aimsLight.sip)
    SET (makeTemplatesGeneratedTypePy ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypesLight.py)
  ELSE (Use_partial_aimssip)
    SET (SIP_MAIN_FILE aims.sip)
    SET (makeTemplatesGeneratedTypePy ${CMAKE_CURRENT_SOURCE_DIR}/generatedtypes.py)
  ENDIF(Use_partial_aimssip)
  
  
  
  
  getFromPro(sip.pro SIP_HDRS SIP_SRCS)
  
  
  SHFJ_COPY_HEADERS(include/pyaims ${SIP_HDRS})
  
  
  # Get the global and relative .sip and .tpl files
  FILE(GLOB TPL_ABSOLUTE          ${CMAKE_CURRENT_SOURCE_DIR}/*.tpl)
  FILE(GLOB TPL_RELATIVE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.tpl)
  FILE(GLOB SIP_ABSOLUTE          ${CMAKE_CURRENT_SOURCE_DIR}/*.sip)
  FILE(GLOB SIP_RELATIVE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.sip)
  
  
  # Copy tpl files for further compilation
  COPY_FILES(${CMAKE_BINARY_DIR}/tpl ${TPL_ABSOLUTE})
  
  
  SET(SIP_TYPESSUB  ${CMAKE_CURRENT_SOURCE_DIR}/typessub.py)
  SET(TPL_DIRECTORY ${CMAKE_BINARY_DIR}/tpl) # -> where to look for tpl files
  
  
  
  # This works in two steps
  # At configure-time, the sip files are instantiated from the .tpl files
  # At configure-time, the .cxx/.h file list only is generated
  # Dependency rules for tpl->sip and sip->cxx/h are defined, with python scripts.
  
  
  # The Macro SIP_MAKE_TEMPLATES creates .sip from .tpl files based on a list of type to
  # be generated. Argument list:
  # 1. Where to look for .tpl files
  # 2. Where to generate .sip files
  # 3. Explicit dependencies: tell the sip generator that if any change occurs
  # to a .tpl file, it should re-make the .sip.
  
  
  SIP_MAKE_TEMPLATES(${makeTemplatesGeneratedTypePy} 
  ${TPL_DIRECTORY} 
  ${CMAKE_CURRENT_BINARY_DIR}/sip 
  ${TPL_ABSOLUTE}
  )
  
  
  # Get the .sip files that have been created
  FILE(GLOB SIPPED_TPL ${CMAKE_CURRENT_BINARY_DIR}/sip/*.sip)
  
  
  # copy the sipped files to a common directory so that external libraries
  # can access them for further compilation
  COPY_FILES(${CMAKE_BINARY_DIR}/sip ${SIPPED_TPL})
  
  
  # save the pyaims sip dir and set it to cache as other libraries depend on it
  SET(PYAIMS_SIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to aims.sip")
  
  
  
  SET(SIP_INCLUDES 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/sip
  )
  SET(_sipSplitGeneratedCode 8)
  SET(SIP_TAGS ALL WS_X11 Qt_3_3_0)
  SET(SIP_DISABLE_FEATURES 
  VendorID 
  Qt_STYLE_WINDOWSXP 
  Qt_STYLE_INTERLACE
  Qt_STYLE_WINDOWSXP
  #CARTODATA # Should we keep it?
  )
  SET(SIP_EXTRA_OPTIONS -e)
  SET(SIP_EXTRA_DEPENDENCIES ${SIPPED_TPL} ${SIP_ABSOLUTE})
  
  
  ADD_SIP_PYTHON_MODULE(aimssip ${SIP_MAIN_FILE}
  graph 
  pyaims 
  aimsdata 
  cartodata 
  cartobase
  )
  
  
  INSTALL(TARGETS aimssip DESTINATION python/soma/aims)

endif( soma-infra_FOUND )
