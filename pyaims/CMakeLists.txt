OPTION( AIMS_PARTIAL_PYAIMSSIP "Use only parts of PyAims, shorter to compile, for devel purpose" OFF )

set( PYAIMS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )

add_subdirectory( src )
add_subdirectory( python )
add_subdirectory( bin )
add_subdirectory( doc )

add_subdirectory( ${aims_SOURCE_DIR}/pyaimsalgo pyaimsalgo )
