add_subdirectory( src )

BRAINVISA_COPY_FILES(${PROJECT_NAME}-devdoc ${CMAKE_CURRENT_SOURCE_DIR}/changelog.html 
  DESTINATION share/doc/cartodata-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}/doxygen )