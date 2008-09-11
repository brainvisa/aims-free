if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/library.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers cartodata cartodata-devel )
  add_library( cartodata SHARED ${_sources} )
  set_property( TARGET cartodata PROPERTY VERSION ${cartodata_VERSION} )
  target_link_libraries( cartodata cartobase )
  SOMA_INSTALL( TARGETS cartodata
                DESTINATION lib
                COMPONENT cartodata )
  SOMA_GENERATE_DOXYGEN_DOC( cartodata-doxygen _headers  )

else( soma-infra_FOUND )

  getFromPro(library.pro Cartodata_HDRS Cartodata_SRCS)
  
  
  SHFJ_COPY_HEADERS(include/cartodata ${Cartodata_HDRS})
  
  
  ADD_LIBRARY(cartodata SHARED
  ${Cartodata_SRCS}
  )
  
  TARGET_LINK_LIBRARIES(cartodata 
  cartobase
  )
  
  #darwin:LIBS      += -framework CoreServices
  ### linux:CXXFLAGS += -march=athlon-xp -mtune=athlon-xp
  #IF (APPLE)
  #TARGET_LINK_LIBRARIES( cartodata cartobase "-framework CoreServices")
  #ELSE(APPLE)
  
  #ENDIF(APPLE)
  
  INSTALL(TARGETS cartodata DESTINATION lib/)

endif( soma-infra_FOUND )
