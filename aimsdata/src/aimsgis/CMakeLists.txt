
set( _sources
     plugin/gisplugin.cc
     plugin/vol_io.cc
)

# BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
# set(AIMSGIS_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsgis SHARED ${_sources} )
# add_dependencies( aimsgis ${headersTarget} )
set_property( TARGET aimsgis PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsgis aimsdata )
BRAINVISA_INSTALL( TARGETS aimsgis
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
