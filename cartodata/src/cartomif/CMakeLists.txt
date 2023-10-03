set( _headers
  plugin/mifplugin.h
)

set( _sources
  plugin/mifplugin.cc
)

add_definitions( "-DHAVE_ZLIB" )

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTOMIF_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartomif MODULE ${_sources} )
add_dependencies( cartomif ${headersTarget} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET cartomif PROPERTY OUTPUT_NAME cartomif.${${PROJECT_NAME}_VERSION} )
elseif( CMAKE_VERSION VERSION_LESS "3.5" )
    set_target_properties( cartomif
        PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( cartomif
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( cartomif cartodata somamif )
BRAINVISA_INSTALL( TARGETS cartomif
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )
