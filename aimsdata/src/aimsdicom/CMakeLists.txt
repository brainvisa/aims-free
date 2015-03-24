
BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/dicom.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSDICOM_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsdicom SHARED ${_sources} )
add_dependencies( aimsdicom ${headersTarget} )
set_property( TARGET aimsdicom PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsdicom aimsdata ${DCMTK_LIBRARIES} ${JASPER_LIBRARIES} ${TIFF_LIBRARIES} ${PNG12_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsdicom
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
