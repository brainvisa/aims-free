
set( _headers
    io/ciftifinderformat.h
    io/ciftiformat.h
    io/ciftiobject.h
    plugin/ciftiplugin.h
)

set( _sources
    io/ciftifinderformat.cc
    io/ciftiformat.cc
    io/ciftiobject.cc
    plugin/ciftiplugin.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSCIFTI_HEADERS ${_headers} PARENT_SCOPE)

add_definitions( ${CIFTI_DEFINITIONS} )
include_directories( ${CIFTI_INCLUDES} )

add_library( aimscifti SHARED ${_sources} )
add_dependencies( aimscifti ${headersTarget} )
set_property( TARGET aimscifti PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimscifti aimsdata ${CIFTI_LIBS} )
BRAINVISA_INSTALL( TARGETS aimscifti
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )

