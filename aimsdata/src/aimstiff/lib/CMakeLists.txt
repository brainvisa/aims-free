BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib-aims-tiff.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
add_library( aimstiff SHARED ${_sources} )
add_dependencies( aimstiff ${headersTarget} )
set_property( TARGET aimstiff PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimstiff aimsdata ${TIFF_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimstiff
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
