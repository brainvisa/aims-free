if( soma-infra_FOUND )

  project( graph )
  SOMA_SET_PROJECT_VERSION()

  SOMA_ADD_COMPONENT( graph 
                      GROUP runtime
                      DESCRIPTION "graph C++ library" )
  SOMA_ADD_COMPONENT( graph-devel 
                      GROUP devel
                      DESCRIPTION "Developpement files for graph"
                      DEPENDS graph )
  SOMA_ADD_COMPONENT( graph-doxygen
                      GROUP doxygen-doc
                      DESCRIPTION "Doxygen documentation of graph" )
  SOMA_ADD_COMPONENT( graph-share
                      GROUP share
                      DESCRIPTION "Shared files of graph" )

  add_subdirectory( src )
  SOMA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/shared"
                       "share/graph-${graph_VERSION_MAJOR}.${graph_VERSION_MINOR}"
                       graph-share graph-share )

else( soma-infra_FOUND )

  PROJECT(GRAPH)
  
  cmake_minimum_required(VERSION 2.4)
  
  IF(COMMAND cmake_policy)
    cmake_policy(VERSION 2.4)
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
  #  cmake_policy(SET CMP0005 OLD)
  ENDIF(COMMAND cmake_policy)
  
  ADD_SUBDIRECTORY(shared)
  ADD_SUBDIRECTORY(src)

endif( soma-infra_FOUND )

