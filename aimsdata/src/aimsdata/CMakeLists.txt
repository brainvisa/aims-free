BRAINVISA_GET_FILE_LIST_FROM_PRO(  ${CMAKE_CURRENT_SOURCE_DIR}/aimsdata.pro "HEADERS" _headers "SOURCES" _sources )
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
if( NOT LIBKDTREE_FOUND )
  # copy internal headers for libkdtree+
  set( _kdtreeHeaders kdtree++/allocator.hpp
    kdtree++/function.hpp
    kdtree++/iterator.hpp
    kdtree++/kdtree.hpp
    kdtree++/node.hpp
    kdtree++/region.hpp
    )
  BRAINVISA_COPY_AND_INSTALL_HEADERS( _kdtreeHeaders "" _kdtreeHeadersTarget )
endif()
set(AIMSDATA_HEADERS ${_headers} PARENT_SCOPE)
add_library( aimsdata SHARED ${_sources} )
add_dependencies( aimsdata ${headersTarget} )
if( NOT LIBKDTREE_FOUND )
  add_dependencies( aimsdata ${_kdtreeHeadersTarget} )
endif()
set_property( TARGET aimsdata PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsdata graph cartodata )
if( WIN32 AND CMAKE_CROSSCOMPILING )
  target_link_libraries( aimsdata gnurx )
endif( WIN32 AND CMAKE_CROSSCOMPILING )
BRAINVISA_INSTALL( TARGETS aimsdata
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
