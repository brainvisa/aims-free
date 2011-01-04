BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aimsgui.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
BRAINVISA_ADD_MOC_FILES( _sources ${_headers} )
add_library( aimsgui SHARED ${_sources} )
add_dependencies( aimsgui ${headersTarget} )
set_property( TARGET aimsgui PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsgui )
BRAINVISA_INSTALL( TARGETS aimsgui
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )

IF ( DOXYGEN_FOUND )
  set(DOXYFILE_PROJECT_NAME "aimsgui")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/aims" )
ENDIF ( DOXYGEN_FOUND )

