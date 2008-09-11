if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers ecat ecat-devel )
  add_library( ecat SHARED ${_sources} )
  set_property( TARGET ecat PROPERTY VERSION ${ecat_VERSION} )
  SOMA_INSTALL( TARGETS ecat
                DESTINATION lib
                COMPONENT ecat )
  SOMA_GENERATE_DOXYGEN_DOC( ecat-doxygen _headers  )

else( soma-infra_FOUND )

  getFromPro(lib.pro ecat_HDRS ecat_SRCS)
  SHFJ_COPY_HEADERS(include/ecat ${ecat_HDRS})
  ADD_LIBRARY(ecat ${ecat_SRCS} )
  TARGET_LINK_LIBRARIES( ecat )
  INSTALL(TARGETS ecat DESTINATION lib/)

endif( soma-infra_FOUND )

