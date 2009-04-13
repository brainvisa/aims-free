BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/public.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsalgopub SHARED ${_sources} )
set_property( TARGET aimsalgopub PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsalgopub aimsdata )
BRAINVISA_INSTALL( TARGETS aimsalgopub
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
