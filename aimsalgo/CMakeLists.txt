if( soma-infra_FOUND )

  project( aimsalgo )
  SOMA_SET_PROJECT_VERSION()

  SOMA_ADD_COMPONENT( aimsalgo 
                      GROUP runtime
                      DESCRIPTION "aimsalgo C++ library" )
  SOMA_ADD_COMPONENT( aimsalgo-devel 
                      GROUP devel
                      DESCRIPTION "Developpement files for aimsalgo"
                      DEPENDS aimsalgo )
  SOMA_ADD_COMPONENT( aimsalgopub-devel 
                      GROUP devel
                      DESCRIPTION "Developpement files for aimsalgopub"
                      DEPENDS aimsalgo )
  SOMA_ADD_COMPONENT( aimsalgo-doxygen
                      GROUP doxygen-doc
                      DESCRIPTION "Doxygen documentation of aimsalgo" )
  SOMA_ADD_COMPONENT( aimsalgo-html
                      GROUP html-doc
                      DESCRIPTION "HTML documentation of aimsalgo" )

  add_subdirectory( src )
  add_subdirectory( doxygen )

else( soma-infra_FOUND )

  PROJECT(AIMSALGO)
  
  cmake_minimum_required(VERSION 2.4)
  
  IF(COMMAND cmake_policy)
    cmake_policy(VERSION 2.4)
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
  #  cmake_policy(SET CMP0005 OLD)
  ENDIF(COMMAND cmake_policy)
  
  
  SUBDIRS(src)

endif( soma-infra_FOUND )
