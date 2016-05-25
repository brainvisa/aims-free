BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/openslide.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTOOPENSLIDE_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartoopenslide MODULE ${_sources} )
add_dependencies( cartoopenslide ${headersTarget} )
include_directories( ${OPENSLIDE_INCLUDE_DIRS} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET cartoopenslide PROPERTY OUTPUT_NAME cartoopenslide.${${PROJECT_NAME}_VERSION} )
elseif( CMAKE_VERSION VERSION_LESS "3.5" )
    set_target_properties( cartoopenslide
        PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( cartoopenslide
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( cartoopenslide cartodata somaopenslide ${OPENSLIDE_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS cartoopenslide
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
