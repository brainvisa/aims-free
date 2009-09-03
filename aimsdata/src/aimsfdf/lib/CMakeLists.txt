BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aims-fdf.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsfdf SHARED ${_sources} )
set_property( TARGET aimsfdf PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsfdf aimsdata )
BRAINVISA_INSTALL( TARGETS aimsfdf
DESTINATION lib
COMPONENT ${PROJECT_NAME} )

