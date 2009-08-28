BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/minc.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims )
include_directories( ${MINC_INCLUDE_DIR} )
add_library( aimsminc SHARED ${_sources} )
set_property( TARGET aimsminc PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsminc aimsdata ${MINC_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsminc
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
