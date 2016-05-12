set( _headers
  plugin/niftiplugin.h
)

set( _sources
  plugin/niftiplugin.cc
)

add_definitions( "-DHAVE_ZLIB" )
include_directories( "${CMAKE_BINARY_DIR}/include/soma-io/nifticlib/znzlib" "${CMAKE_BINARY_DIR}/include/soma-io/nifticlib/niftilib" )

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTONIFTI_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartonifti MODULE ${_sources} )
add_dependencies( cartonifti ${headersTarget} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET cartonifti PROPERTY OUTPUT_NAME cartonifti.${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( cartonifti
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( cartonifti cartodata somanifti )
BRAINVISA_INSTALL( TARGETS cartonifti
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
