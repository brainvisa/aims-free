
BRAINVISA_GET_FILE_LIST_FROM_PRO( ${CMAKE_CURRENT_SOURCE_DIR}/dicom.pro
                                  "HEADERS" _headers
                                  "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSDICOM_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsdicom SHARED ${_sources} )

if ( NOT WIN32 )
  add_definitions( -DHAVE_CONFIG_H )
# Not necessary anymore (probably for version >= 3.6.0)
# it generates warning during compilation
# else ()
#   add_definitions( -DUSE_STD_CXX_INCLUDES )
endif ()

add_dependencies( aimsdicom ${headersTarget} )
set_property( TARGET aimsdicom PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
# For DICOM version < 3.6, dcmimage.so depends on PNG and TIFF libraries
# but does not refer these libraries (especially on Ubuntu 12 and 14).
# DCMTK_LIBRARIES_ADDITIONAL variable is used to avoid undefined symbols.
target_link_libraries( aimsdicom aimsdata
                       ${DCMTK_LIBRARIES}
                       ${DCMTK_LIBRARIES_ADDITIONAL} )
BRAINVISA_INSTALL( TARGETS aimsdicom
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
