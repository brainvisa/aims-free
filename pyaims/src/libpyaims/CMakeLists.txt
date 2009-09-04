BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/pyaims.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers pyaims headersTarget )
add_library( pyaims SHARED ${_sources} )
add_dependencies( pyaims ${headersTarget} )
set_property( TARGET pyaims PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( pyaims cartobase ${PYTHON_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS pyaims
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
