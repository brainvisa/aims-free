if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/pyaims.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers pyaims aims-devel )
  add_library( pyaims SHARED ${_sources} )
  set_property( TARGET pyaims PROPERTY VERSION ${pyaims_VERSION} )
  target_link_libraries( pyaims cartobase ${PYTHON_LIBRARIES} )
  SOMA_INSTALL( TARGETS pyaims
                DESTINATION lib
                COMPONENT aims )
  
else( soma-infra_FOUND )

  getFromPro(pyaims.pro PYAIMS_HDRS PYAIMS_SRCS)
  
  SHFJ_COPY_HEADERS(include/pyaims ${PYAIMS_HDRS})
  
  ADD_LIBRARY(pyaims SHARED 
  ${PYAIMS_SRCS}
  )
  
  
  TARGET_LINK_LIBRARIES(pyaims
  cartobase
  ${PYTHON_LIBRARY}
  )
  
  
  INSTALL(TARGETS pyaims DESTINATION lib/)

endif( soma-infra_FOUND )
