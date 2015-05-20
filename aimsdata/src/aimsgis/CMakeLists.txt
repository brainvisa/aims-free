
set( _sources
     plugin/gisplugin.cc
     plugin/vol_io.cc
)

# BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
# set(AIMSGIS_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsgis MODULE ${_sources} )
# add_dependencies( aimsgis ${headersTarget} )

if( NOT APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET aimsgis PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
else()
  set_property( TARGET aimsgis PROPERTY OUTPUT_NAME aimsgis.${${PROJECT_NAME}_VERSION} )
endif()

target_link_libraries( aimsgis aimsdata )
BRAINVISA_INSTALL( TARGETS aimsgis
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
