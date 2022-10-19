BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/lib.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers vidaIO headersTarget )
add_library( vidaIO SHARED ${_sources} )
add_dependencies( vidaIO ${headersTarget} )
set_property( TARGET vidaIO PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( vidaIO m )
BRAINVISA_INSTALL( TARGETS vidaIO
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )


# The rpc/rpc.h, rpc/types.h and rpc/xdr.h headers used to be shipped with
# glibc, but they were moved to libtirpc and removed in glibc 2.26 (released
# August 2017).
include(CheckIncludeFiles)
CHECK_INCLUDE_FILES("rpc/types.h;rpc/xdr.h" rpc_headers_present)
if(NOT rpc_headers_present)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(libtirpc REQUIRED libtirpc)
  set_target_properties( vidaIO PROPERTIES
    COMPILE_FLAGS "${libtirpc_CFLAGS}"
    LINK_FLAGS "${libtirpc_LDFLAGS}"
    )
  # TODO BrainVISA packaging for libtirpc
endif()


IF ( DOXYGEN_FOUND )
  set(component_name "vidaIO")
  set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
  BRAINVISA_GENERATE_DOXYGEN_DOC( _headers
                                  INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/${component_name}"
                                  COMPONENT "${component_name}" )
ENDIF ( DOXYGEN_FOUND )
