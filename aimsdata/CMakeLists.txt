add_subdirectory( src )
SOMA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/shared" 
                      "share/aimsdata-${aimsdata_VERSION_MAJOR}.${aimsdata_VERSION_MINOR}"
                      ${PROJECT_NAME}-share )
