if( soma-infra_FOUND )

#   project( pyaimsalgo )
#   SOMA_SET_PROJECT_VERSION()
# 
#   SOMA_ADD_COMPONENT( pyaimsalgo 
#                       GROUP runtime
#                       DESCRIPTION "pyaimsalgo C++ library" )
#   SOMA_ADD_COMPONENT( pyaimsalgo-devel 
#                       GROUP devel
#                       DESCRIPTION "Developpement files for pyaimsalgo"
#                       DEPENDS pyaims )
#   SOMA_ADD_COMPONENT( pyaimsalgo-epydoc
#                       GROUP epydoc-doc
#                       DESCRIPTION "Epydoc documentation of pyaimsalgo" )
  
  add_subdirectory( src )
  add_subdirectory( python )
  add_subdirectory( bin )
  add_subdirectory( doc )

else( soma-infra_FOUND )

  PROJECT(PYAIMSALGO)
  
  
  ADD_SUBDIRECTORY(src)
  ADD_SUBDIRECTORY(bin)
  ADD_SUBDIRECTORY(python)
  ADD_SUBDIRECTORY(doc)

endif( soma-infra_FOUND )
