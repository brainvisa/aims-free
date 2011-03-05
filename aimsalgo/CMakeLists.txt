if( LIBKDTREE_FOUND )
  add_definitions( "-DCARTO_USE_KDTREE" )
  include_directories( ${LIBKDTREE_INCLUDE_DIR} )
endif()

add_subdirectory( src )
add_subdirectory( doxygen )
