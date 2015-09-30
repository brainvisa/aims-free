# Ensure that features such as strdup and caddr_t are defined when GCC/clang
# is used in strict standards mode (-std=c89)
add_definitions( -D_DEFAULT_SOURCE )

BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers ecat headersTarget )
add_library( ecat SHARED ${_sources} )
add_dependencies( ecat ${headersTarget} )
set_property( TARGET ecat PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( ecat "m" )
BRAINVISA_INSTALL( TARGETS ecat
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
              
IF ( DOXYGEN_FOUND )
  set(component_name "ecat")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers 
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}" 
                                  COMPONENT "${component_name}")
ENDIF ( DOXYGEN_FOUND )

