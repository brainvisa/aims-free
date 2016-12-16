BRAINVISA_ADD_EXECUTABLE(AimsStructuringElement_test     AimsStructuringElement_test/structuringelement_test.cc HELP_GENERATE FALSE)

TARGET_LINK_LIBRARIES(AimsStructuringElement_test        ${AIMS_AIMSDATA_LIBRARIES})

BRAINVISA_ADD_EXECUTABLE(AimsPartialIO_test   AimsPartialIO_test/partialio_test.cc HELP_GENERATE FALSE)
BRAINVISA_ADD_EXECUTABLE(AimsVarious_test     AimsVarious_test/various_test.cc HELP_GENERATE FALSE)

TARGET_LINK_LIBRARIES(AimsPartialIO_test   ${AIMS_AIMSDATA_LIBRARIES})
TARGET_LINK_LIBRARIES(AimsVarious_test     ${AIMS_AIMSDATA_LIBRARIES})

BRAINVISA_INSTALL( TARGETS AimsStructuringElement_test AimsPartialIO_test
              AimsVarious_test
              DESTINATION bin
              COMPONENT ${PROJECT_NAME}-test )

IF ( PYTHON_FOUND )
  # Run tests for aims using python unittest
  set( _bv_real_bin "\${BRAINVISA_REAL_BIN}" )
  add_test(AimsStructuringElementTests
    "${CMAKE_BINARY_DIR}/bin/bv_env_test"
    "${PYTHON_EXECUTABLE}"
    "$BRAINVISA_PACKAGE_INSTALL_PREFIX/bin/\\\${BRAINVISA_REAL_BIN}bv_unit_test"
    aims.tests.test_aimsdata.AimsStructuringElementTests)
ENDIF()
