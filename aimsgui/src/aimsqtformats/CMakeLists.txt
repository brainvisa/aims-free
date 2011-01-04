add_definitions( ${QT_DEFINITIONS} )
include_directories( ${QT_INCLUDE_DIR} )

BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/qtformats.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
add_library( aimsqtformats SHARED ${_sources} )
add_dependencies( aimsqtformats ${headersTarget} )
set_property( TARGET aimsqtformats PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsqtformats aimsdata ${QT_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsqtformats
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
