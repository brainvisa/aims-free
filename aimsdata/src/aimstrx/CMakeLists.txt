set( _headers
    fibers/trxbundlereader.h
    plugin/trxplugin.h
)

set( _sources
    fibers/trxbundlereader.cc
    plugin/trxplugin.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSTRX_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimstrx MODULE ${_sources} )
add_dependencies( aimstrx ${headersTarget} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET aimstrx PROPERTY OUTPUT_NAME aimstrx.${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has to be forced.
  set_target_properties( aimstrx
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries(aimstrx ${AIMS_AIMSDATA_LIBRARIES} trx-cpp::trx)
BRAINVISA_INSTALL(TARGETS aimstrx
    DESTINATION lib
    COMPONENT ${PROJECT_NAME})
