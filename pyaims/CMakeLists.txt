if( soma-infra_FOUND )

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

  add_subdirectory( $ENV{P4}/pyaimsalgo-main pyaimsalgo-main )

else( soma-infra_FOUND )
  PROJECT(PYAIMS)
  
  IF(COMMAND cmake_policy)
    cmake_policy(VERSION 2.4)
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
  #  cmake_policy(SET CMP0005 OLD)
  ENDIF(COMMAND cmake_policy)
  
  ADD_SUBDIRECTORY(src)
  ADD_SUBDIRECTORY(python)
  ADD_SUBDIRECTORY(bin)
  ADD_SUBDIRECTORY(doc)

endif( soma-infra_FOUND )
