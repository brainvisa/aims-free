configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/config/version.h.in" "${CMAKE_BINARY_DIR}/include/cartobase/config/version.h" @ONLY )
BRAINVISA_INSTALL( FILES "${CMAKE_BINARY_DIR}/include/cartobase/config/version.h" 
                   DESTINATION "include/cartobase/config" 
                   COMPONENT ${PROJECT_NAME}-dev )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.in" "${CMAKE_BINARY_DIR}/include/cartobase/config/config.h" @ONLY )
BRAINVISA_INSTALL( FILES "${CMAKE_BINARY_DIR}/include/cartobase/config/config.h"
                   DESTINATION "include/cartobase/config"
                   COMPONENT ${PROJECT_NAME}-dev )

BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartobase headersTarget )
add_library( cartobase_0 SHARED ${_sources} )
add_dependencies( cartobase_0 ${headersTarget} )
set_property( TARGET cartobase_0 PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartobase_0 ${LIBXML2_LIBRARIES} ${Sigc++2_LIBRARIES}  )
if( WIN32 AND CMAKE_CROSSCOMPILING )
  target_link_libraries( cartobase_0 ws2_32 ole32 )
endif( WIN32 AND CMAKE_CROSSCOMPILING )
if( WIN32 )
  target_link_libraries( cartobase_0 wsock32 )
endif( WIN32 )
if(UNIX)
  target_link_libraries( cartobase_0 "m" "pthread" )
endif(UNIX)
if(APPLE)
  target_link_libraries( cartobase_0 "-framework CoreServices" )
endif(APPLE)
BRAINVISA_INSTALL( TARGETS cartobase_0
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )

IF ( DOXYGEN_FOUND )
  SET(component_name "cartobase")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}"
                                  COMPONENT "${component_name}")
ENDIF ( DOXYGEN_FOUND )
