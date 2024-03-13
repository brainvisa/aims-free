set( _headers
    io/gifticlib/gifti_io.h
    io/gifticlib/gifti_xml.h
    io/gifti.h
    io/giftiformat.h
    io/giftimeshformat_d.h
    io/giftitextureformat_d.h
    io/giftiheader.h
    io/giftifinderformat.h
    io/giftiutil.h
    plugin/giftiplugin.h
)

set( _sources
    io/gifti.cc
    io/giftiformat.cc
    io/giftiheader.cc
    io/giftiutil.cc
    plugin/giftiplugin.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSGIFTI_HEADERS ${_headers} PARENT_SCOPE)
include_directories( "${CMAKE_BINARY_DIR}/include/aims/io/gifticlib" "${CMAKE_BINARY_DIR}/include/soma-io/nifticlib/niftilib" "${CMAKE_BINARY_DIR}/include/soma-io/nifticlib/znzlib" ${EXPAT_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} )
add_definitions( "-DHAVE_ZLIB" )
add_library( aimsgifti MODULE ${_sources} )
add_dependencies( aimsgifti ${headersTarget} )

if( APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET aimsgifti PROPERTY OUTPUT_NAME aimsgifti.${${PROJECT_NAME}_VERSION} )
elseif( CMAKE_VERSION VERSION_LESS "3.5" )
    set_target_properties( aimsgifti
        PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} )
else()
  # in cmake 3.5 on linux, making a "module" removes the version number in the
  # .so so the suffix has to be forced.
  set_target_properties( aimsgifti
    PROPERTIES SOVERSION ${${PROJECT_NAME}_VERSION} SUFFIX ".so.${${PROJECT_NAME}_VERSION}" )
endif()

target_link_libraries( aimsgifti ${AIMS_AIMSDATA_LIBRARIES} somanifti "${ZLIB_LIBRARIES}" ${EXPAT_LIBRARIES} )
BRAINVISA_INSTALL( TARGETS aimsgifti
DESTINATION lib
COMPONENT ${PROJECT_NAME} )
