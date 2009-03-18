include_directories( ${DCMTK_INCLUDE_DIR} )

SOMA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/dicom.pro "HEADERS" _headers "SOURCES" _sources )
SOMA_COPY_AND_INSTALL_HEADERS( _headers aims ${PROJECT_NAME}-devel )
add_library( aimsdicom SHARED ${_sources} )
set_property( TARGET aimsdicom PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsdicom aimsdata ${DCMTK_LIBRARIES} )
SOMA_INSTALL( TARGETS aimsdicom
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
