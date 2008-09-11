if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers graph graph-devel )
  add_library( graph SHARED ${_sources} )
  set_property( TARGET graph PROPERTY VERSION ${graph_VERSION} )
  target_link_libraries( graph cartobase )
  SOMA_INSTALL( TARGETS graph
                DESTINATION lib
                COMPONENT graph )
  SOMA_GENERATE_DOXYGEN_DOC( graph-doxygen _headers  )


else( soma-infra_FOUND )

  getFromPro(lib.pro graph_HDRS graph_SRCS)
  
  SHFJ_COPY_HEADERS(include/graph ${graph_HDRS})
  
  ADD_LIBRARY(graph SHARED 
  ${graph_SRCS}
  )
  
  
  TARGET_LINK_LIBRARIES(graph
  cartobase
  )
  
  INSTALL(TARGETS graph DESTINATION lib/)

endif( soma-infra_FOUND )
