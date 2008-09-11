if( soma-infra_FOUND )

  project( vidaIO )
  SOMA_SET_PROJECT_VERSION()

  SOMA_ADD_COMPONENT( vidaIO 
                      GROUP runtime
                      DESCRIPTION "vidaIO C++ library" )
  SOMA_ADD_COMPONENT( vidaIO-devel 
                      GROUP devel
                      DESCRIPTION "Developpement files for vidaIO"
                      DEPENDS vidaIO )
  SOMA_ADD_COMPONENT( vidaIO-doxygen
                      GROUP doxygen-doc
                      DESCRIPTION "Doxygen documentation of vidaIO" )
  SOMA_ADD_COMPONENT( vidaIO-share
                      GROUP share
                      DESCRIPTION "Shared files of vidaIO" )

  add_subdirectory( src )

else( soma-infra_FOUND )

  PROJECT(VIDAIO)
  
  ADD_SUBDIRECTORY(src)

endif( soma-infra_FOUND )
