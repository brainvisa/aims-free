set( _headers
  plugin/dicomplugin.h
)

set( _sources
  plugin/dicomplugin.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers cartodata headersTarget )
set(CARTODICOM_HEADERS ${_headers} PARENT_SCOPE)
add_library( cartodicom MODULE ${_sources} )
add_dependencies( cartodicom ${headersTarget} )
if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET cartodicom PROPERTY OUTPUT_NAME cartodicom.${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( cartodicom
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( cartodicom cartodata somadicom )
BRAINVISA_INSTALL( TARGETS cartodicom
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )

