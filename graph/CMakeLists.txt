add_subdirectory( src )
add_subdirectory( doxygen )
BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/shared"
                          "share/graph-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}"
                          ${PROJECT_NAME} )
