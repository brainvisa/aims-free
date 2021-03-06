BRAINVISA_ADD_EXECUTABLE(AimsStructuringElement_test     AimsStructuringElement_test/structuringelement_test.cc HELP_GENERATE FALSE)

TARGET_LINK_LIBRARIES(AimsStructuringElement_test        ${AIMS_AIMSDATA_LIBRARIES})

BRAINVISA_ADD_EXECUTABLE(AimsPartialIO_test   AimsPartialIO_test/partialio_test.cc HELP_GENERATE FALSE)
BRAINVISA_ADD_EXECUTABLE(AimsVarious_test     AimsVarious_test/various_test.cc HELP_GENERATE FALSE)
BRAINVISA_ADD_EXECUTABLE(AimsTransformation_test AimsTransformation_test/transformation_test.cc HELP_GENERATE FALSE)

TARGET_LINK_LIBRARIES(AimsPartialIO_test   ${AIMS_AIMSDATA_LIBRARIES})
TARGET_LINK_LIBRARIES(AimsVarious_test     ${AIMS_AIMSDATA_LIBRARIES})
TARGET_LINK_LIBRARIES(AimsTransformation_test ${AIMS_AIMSDATA_LIBRARIES})

BRAINVISA_INSTALL( TARGETS AimsStructuringElement_test AimsPartialIO_test
              AimsVarious_test
              AimsTransformation_test
              DESTINATION bin
              COMPONENT ${PROJECT_NAME}-test )

brainvisa_add_test(NAME AimsTransformationTests
                   COMMAND AimsTransformation_test)

IF ( PYTHON_FOUND )
  # Run tests for aims using python unittest
  brainvisa_add_test(AimsStructuringElementTests
  "${PYTHON_EXECUTABLE_NAME}" -m aims.tests.test_aimsdata TESTREF)
ENDIF()
