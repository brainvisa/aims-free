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
set_property( TARGET cartodicom PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( cartodicom cartodata )
BRAINVISA_INSTALL( TARGETS cartodicom
                   DESTINATION lib
                   COMPONENT ${PROJECT_NAME} )

