SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/nifti.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims )
include_directories( "${CMAKE_BINARY_DIR}/include/aims/io/znzlib" )
add_library( aimsnifti SHARED ${_sources} )
set_property( TARGET aimsnifti PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsnifti aimsdata ${NIFTI_LIBRARIES} )
SOMA_INSTALL( TARGETS aimsnifti
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
