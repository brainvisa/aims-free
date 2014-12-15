add_subdirectory( src )
add_subdirectory( doc )
add_subdirectory( doxygen )
BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/shared" 
                          "share/aims-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}"
                          ${PROJECT_NAME} )

find_package( Sphinx )
BRAINVISA_GENERATE_SPHINX_DOC( "sphinx/user_doc"
  "share/doc/aimsdata-${BRAINVISA_PACKAGE_VERSION_MAJOR}.${BRAINVISA_PACKAGE_VERSION_MINOR}/user_doc" USER )

