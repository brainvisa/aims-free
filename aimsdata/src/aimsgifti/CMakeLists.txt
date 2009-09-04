BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/gifti.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
include_directories( "${CMAKE_BINARY_DIR}/include/aims/io/gifticlib" "${CMAKE_BINARY_DIR}/include/aims/io/niftilib" "${CMAKE_BINARY_DIR}/include/aims/io/znzlib" ${EXPAT_INCLUDE_DIRS} )
add_library( aimsgifti SHARED ${_sources} )
add_dependencies( aimsgifti ${headersTarget} )
set_property( TARGET aimsgifti PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsgifti aimsdata ${EXPAT_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsgifti
DESTINATION lib
COMPONENT ${PROJECT_NAME} )
