if( soma-infra_FOUND )

  SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aimsgui.pro "HEADERS" _headers "SOURCES" _sources )
  SOMA_COPY_AND_INSTALL_HEADERS( _headers aims aimsgui-devel )
  # try to moc every .h, moc is smart enough not to crash if the .h doesn't need
  # it, it will just generate harmless empty .moc.cpp files
  SOMA_ADD_MOC_FILES( _sources ${_headers} )
  add_library( aimsgui SHARED ${_sources} )
  set_property( TARGET aimsgui PROPERTY VERSION ${aimsdata_VERSION} )
  target_link_libraries( aimsgui ${QT_LIBRARIES} aimsdata )
  SOMA_INSTALL( TARGETS aimsgui
                DESTINATION lib
                COMPONENT aimsdata )

else( soma-infra_FOUND )
  
  getFromPro(lib-aimsgui.pro HEADERS SOURCES)
  
  SHFJ_COPY_HEADERS(include/aims ${HEADERS})
  
  # try to moc every .h, moc is smart enough not to crash if the .h doesn't need
  # it, it will just generate harmless empty .moc.cpp files
  kde3_add_moc_files(SOURCES ${HEADERS})
  
  
  add_library(aimsgui SHARED 
  ${SOURCES}
  )
  
  TARGET_LINK_LIBRARIES(aimsgui
  ${QT_LIBRARIES}
  aimsdata
  )
  
  INSTALL(TARGETS aimsgui DESTINATION lib/)

endif( soma-infra_FOUND )
