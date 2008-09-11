if( soma-infra_FOUND )

  project( ecat+ )
  SOMA_SET_PROJECT_VERSION()

  SOMA_ADD_COMPONENT( ecat+ 
                      GROUP runtime
                      DESCRIPTION "ecat+ C++ library" )
  SOMA_ADD_COMPONENT( ecat+-devel 
                      GROUP devel
                      DESCRIPTION "Developpement files for ecat+"
                      DEPENDS ecat+ )
  SOMA_ADD_COMPONENT( ecat+-doxygen
                      GROUP doxygen-doc
                      DESCRIPTION "Doxygen documentation of ecat+" )
  SOMA_ADD_COMPONENT( ecat+-share
                      GROUP share
                      DESCRIPTION "Shared files of ecat+" )

  add_subdirectory( src )

else( soma-infra_FOUND )

  PROJECT(ECAT+)
  
  cmake_minimum_required(VERSION 2.4)
  
  IF(COMMAND cmake_policy)
    cmake_policy(VERSION 2.4)
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
  #  cmake_policy(SET CMP0005 OLD)
  ENDIF(COMMAND cmake_policy)
  
  ADD_SUBDIRECTORY(src)

endif( soma-infra_FOUND )
