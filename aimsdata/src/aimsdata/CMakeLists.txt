BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsdata.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsdata SHARED ${_sources} )
set_property( TARGET aimsdata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsdata graph cartodata )
if( WIN32 AND CMAKE_CROSSCOMPILING )
  target_link_libraries( aimsdata gnurx )
endif( WIN32 AND CMAKE_CROSSCOMPILING )
BRAINVISA_INSTALL( TARGETS aimsdata
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
