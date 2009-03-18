add_subdirectory( src )
SOMA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/shared"
                      "share/graph-${graph_VERSION_MAJOR}.${graph_VERSION_MINOR}"
                      aims-graph-share graph-share )
