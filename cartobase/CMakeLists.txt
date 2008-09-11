if( soma-infra_FOUND )

  project( cartobase )
  SOMA_SET_PROJECT_VERSION()

  SOMA_ADD_COMPONENT( cartobase 
                      GROUP runtime
                      DESCRIPTION "cartobase C++ library" )
  SOMA_ADD_COMPONENT( cartobase-devel
                      GROUP devel
                      DESCRIPTION "Developpement files for cartobase"
                      DEPENDS cartobase )
  SOMA_ADD_COMPONENT( cartobase-doxygen
                      GROUP doxygen-doc
                      DESCRIPTION "Doxygen documentation of cartobase" )

  add_subdirectory( src )

else( soma-infra_FOUND )
  PROJECT(CARTOBASE)
  
  cmake_minimum_required(VERSION 2.4)
  
  IF(COMMAND cmake_policy)
    cmake_policy(VERSION 2.4)
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
  #  cmake_policy(SET CMP0005 OLD)
  ENDIF(COMMAND cmake_policy)
  ADD_SUBDIRECTORY(src)
endif( soma-infra_FOUND )


