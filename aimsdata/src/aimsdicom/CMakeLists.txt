include_directories( ${DCMTK_INCLUDE_DIR} )

BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/dicom.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims )
add_library( aimsdicom SHARED ${_sources} )
set_property( TARGET aimsdicom PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsdicom aimsdata ${DCMTK_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsdicom
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
