BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/libobjects.pro "HEADERS" _headers "SOURCES" _sources )
# BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartobase )
add_library( cartobase_1 SHARED ${_sources} )
set_property( TARGET cartobase_1 PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartobase_1 "cartobase_0" ${LIBXML2_LIBRARIES} ${Sigc++2_LIBRARIES} )
if(UNIX)
  target_link_libraries( cartobase_1 "pthread" )
endif(UNIX)
BRAINVISA_INSTALL( TARGETS cartobase_1
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
