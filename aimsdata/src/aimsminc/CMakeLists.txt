SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/minc.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims ${PROJECT_NAME}-devel )
add_library( aimsminc SHARED ${_sources} )
set_property( TARGET aimsminc PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsminc aimsdata ${MINC_LIBRARIES} )
SOMA_INSTALL( TARGETS aimsminc
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
