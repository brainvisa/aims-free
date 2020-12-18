
set( _headers
     plugin/pyaimsplugin.h
   )

set( _sources
     plugin/pyaimsplugin.cc
   )

set(PYAIMS_HEADERS ${_headers} PARENT_SCOPE)
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers pyaims headersTarget )

add_library( pyaimsplugin MODULE ${_sources} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET pyaimsplugin PROPERTY OUTPUT_NAME pyaimsplugin.${${PROJECT_NAME}_VERSION} )
elseif( CMAKE_VERSION VERSION_LESS "3.5" )
    set_target_properties( pyaimsplugin
        PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has top be forced.
  set_target_properties( pyaimsplugin
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( pyaimsplugin aimsdata "${PYTHON_LIBRARY}" )
BRAINVISA_INSTALL( TARGETS pyaimsplugin
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
