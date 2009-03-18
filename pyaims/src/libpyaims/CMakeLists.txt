SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/pyaims.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers pyaims ${PROJECT_NAME}-devel )
add_library( pyaims SHARED ${_sources} )
set_property( TARGET pyaims PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( pyaims cartobase ${PYTHON_LIBRARIES} )
SOMA_INSTALL( TARGETS pyaims
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
