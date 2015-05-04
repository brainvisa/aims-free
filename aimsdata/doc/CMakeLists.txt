BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/html"
                          "share/doc/aimsdata-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}/html"
                          ${PROJECT_NAME}-usrdoc )

BRAINVISA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/code_examples"
                          "share/doc/aimsdata-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}/code_examples"
                          ${PROJECT_NAME}-usrdoc )

