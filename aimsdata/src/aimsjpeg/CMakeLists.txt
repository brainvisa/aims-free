BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/jpeg.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSJPEG_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsjpeg SHARED ${_sources} )
add_dependencies( aimsjpeg ${headersTarget} )
set_property( TARGET aimsjpeg PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsjpeg aimsdata ${JPEG_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsjpeg
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
