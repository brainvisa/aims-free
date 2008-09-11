if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers vidaIO vidaIO-devel )
  add_library( vidaIO SHARED ${_sources} )
  set_property( TARGET vidaIO PROPERTY VERSION ${vidaIO_VERSION} )
  target_link_libraries( vidaIO m )
  SOMA_INSTALL( TARGETS vidaIO
                DESTINATION lib
                COMPONENT vidaIO )
  SOMA_GENERATE_DOXYGEN_DOC( vidaIO-doxygen _headers  )

else( soma-infra_FOUND )

  getFromPro(lib.pro VIDAIO_HDRS VIDAIO_SRCS)
  
  SHFJ_COPY_HEADERS(include/vidaIO ${VIDAIO_HDRS})
  
  ADD_LIBRARY(vidaIO SHARED
  ${VIDAIO_SRCS}
  )
  
  TARGET_LINK_LIBRARIES(vidaIO 
  m
  )
  
  INSTALL(TARGETS vidaIO DESTINATION lib/)

endif( soma-infra_FOUND )
