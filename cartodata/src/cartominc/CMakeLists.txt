set( _headers
  plugin/mincplugin.h
)

set( _sources
  plugin/mincplugin.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTOMINC_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartominc MODULE ${_sources} )
add_dependencies( cartominc ${headersTarget} )
add_definitions( ${LIBMINC_DEFINITIONS} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET cartominc PROPERTY OUTPUT_NAME cartominc.${${PROJECT_NAME}_VERSION} )
elseif( CMAKE_VERSION VERSION_LESS "3.5" )
    set_target_properties( cartominc
        PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( cartominc
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( cartominc cartodata somaminc )
BRAINVISA_INSTALL( TARGETS cartominc
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
