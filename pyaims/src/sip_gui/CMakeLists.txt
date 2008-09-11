if( soma-infra_FOUND )

  SOMA_ADD_SIP_PYTHON_MODULE( aimsguisip soma/aims aims aims-devel aimsgui.sip )
  target_link_libraries( aimsguisip ${PYTHON_LIBRARY} cartobase cartodata graph aimsdata aimsgui pyaims )

else( soma-infra_FOUND )

  SET(SIP_MAIN_FILE aimsgui.sip)
  
  SET(SIP_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
  SET(SIP_CONCAT_PARTS 1)
  SET(SIP_TAGS ALL WS_X11 Qt_3_3_0)
  SET(SIP_DISABLE_FEATURES 
  VendorID 
  Qt_STYLE_WINDOWSXP 
  Qt_STYLE_INTERLACE 
  Qt_STYLE_WINDOWSXP
  )
  SET(SIP_EXTRA_OPTIONS -e)
  
  ADD_SIP_PYTHON_MODULE(aimsguisip ${SIP_MAIN_FILE} 
  graph 
  pyaims 
  aimsgui 
  aimsdata 
  cartodata
  cartobase
  )
  
  INSTALL(TARGETS aimsguisip DESTINATION python/soma/aims)

endif( soma-infra_FOUND )
