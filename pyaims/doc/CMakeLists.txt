BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/html"
                          "share/doc/pyaims-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}/html"
                          ${PROJECT_NAME}-usrdoc )
BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/examples"
                          "share/doc/pyaims-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}/examples"
                          ${PROJECT_NAME}-usrdoc )
