if( soma-infra_FOUND )

  project( cartodata )
  SOMA_SET_PROJECT_VERSION()

  SOMA_ADD_COMPONENT( cartodata 
                      GROUP runtime
                      DESCRIPTION "cartodata C++ library" )
  SOMA_ADD_COMPONENT( cartodata-devel
                      GROUP devel
                      DESCRIPTION "Developpement files for cartodata"
                      DEPENDS cartodata )
  SOMA_ADD_COMPONENT( cartodata-doxygen
                      GROUP doxygen-doc
                      DESCRIPTION "Doxygen documentation of cartodata" )

  add_subdirectory( src )

else( soma-infra_FOUND )
  PROJECT(CARTODATA)
  
  cmake_minimum_required(VERSION 2.4)
  
  IF(COMMAND cmake_policy)
    cmake_policy(VERSION 2.4)
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
  #  cmake_policy(SET CMP0005 OLD)
  ENDIF(COMMAND cmake_policy)
  
  ADD_SUBDIRECTORY(src)
endif( soma-infra_FOUND )
