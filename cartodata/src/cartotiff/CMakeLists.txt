set( _headers
  plugin/tiffplugin.h
)

set( _sources
  plugin/tiffplugin.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTOTIFF_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartotiff MODULE ${_sources} )
add_dependencies( cartotiff ${headersTarget} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET cartotiff PROPERTY OUTPUT_NAME cartotiff.${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( cartotiff
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( cartotiff cartodata somatiff )
BRAINVISA_INSTALL( TARGETS cartotiff
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
