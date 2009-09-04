BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/nifti.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
include_directories( "${CMAKE_BINARY_DIR}/include/aims/io/znzlib" "${CMAKE_BINARY_DIR}/include/aims/io/niftilib" )
add_library( aimsnifti SHARED ${_sources} )
add_dependencies( aimsnifti ${headersTarget} )
set_property( TARGET aimsnifti PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsnifti aimsdata )
BRAINVISA_INSTALL( TARGETS aimsnifti
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
