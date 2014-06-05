BRAINVISA_ADD_EXECUTABLE(AimsStructuringElement_test     AimsStructuringElement_test/structuringelement_test.cc HELP_GENERATE FALSE)

TARGET_LINK_LIBRARIES(AimsStructuringElement_test        ${AIMS_AIMSDATA_LIBRARIES})

BRAINVISA_ADD_EXECUTABLE(AimsPartialIO_test   AimsPartialIO_test/partialio_test.cc HELP_GENERATE FALSE)
BRAINVISA_ADD_EXECUTABLE(AimsVarious_test     AimsVarious_test/various_test.cc HELP_GENERATE FALSE)

TARGET_LINK_LIBRARIES(AimsPartialIO_test   ${AIMS_AIMSDATA_LIBRARIES})
TARGET_LINK_LIBRARIES(AimsVarious_test     ${AIMS_AIMSDATA_LIBRARIES})

IF ( PYTHON_FOUND )
  # Run tests for aims using python unittest
  ADD_TEST(AimsStructuringElementTests "${CMAKE_BINARY_DIR}/bin/bv_env" "${PYTHON_EXECUTABLE}"
                                       "${CMAKE_BINARY_DIR}/bin/bv_unit_test" 
                                       aims.tests.test_aimsdata.AimsStructuringElementTests)
ENDIF()