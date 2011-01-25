BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aimsgui.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
BRAINVISA_ADD_MOC_FILES( _sources ${_headers} )
add_library( aimsgui SHARED ${_sources} )
add_dependencies( aimsgui ${headersTarget} )
set_property( TARGET aimsgui PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsgui ${QT_LIBRARIES} aimsdata )
BRAINVISA_INSTALL( TARGETS aimsgui
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )

IF ( DOXYGEN_FOUND )
  set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
  set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/graph-${aims_version}/doxygen/graph.tag=../../graph-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/cartodata-${aims_version}/doxygen/cartodata.tag=../../cartodata-${aims_version}/doxygen ${CMAKE_BINARY_DIR}/share/doc/aimsdata-${aims_version}/doxygen/aimsdata.tag=../../aimsdata-${aims_version}/doxygen")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/aims" 
                                  COMPONENT aimsgui )
  add_dependencies( aimsgui-doxygen graph-doxygen cartobase-doxygen cartodata-doxygen aimsdata-doxygen )
ENDIF ( DOXYGEN_FOUND )

