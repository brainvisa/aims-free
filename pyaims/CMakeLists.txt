#   project( pyaims )
#   SOMA_SET_PROJECT_VERSION()
# 
#   SOMA_ADD_COMPONENT( pyaims 
#                       GROUP runtime
#                       DESCRIPTION "pyaims C++ library" )
#   SOMA_ADD_COMPONENT( pyaims-devel 
#                       GROUP devel
#                       DESCRIPTION "Developpement files for pyaims"
#                       DEPENDS pyaims )
#   SOMA_ADD_COMPONENT( pyaims-epydoc
#                       GROUP epydoc-doc
#                       DESCRIPTION "Epydoc documentation of pyaims" )

OPTION( AIMS_PARTIAL_PYAIMSSIP "Use only parts of PyAims, shorter to compile, for devel purpose" OFF )

set( PYAIMS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )

add_subdirectory( src )
add_subdirectory( python )
add_subdirectory( bin )
add_subdirectory( doc )

add_subdirectory( ${aims_SOURCE_DIR}/pyaimsalgo pyaimsalgo )
