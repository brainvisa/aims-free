
include_directories( BEFORE ${PYAIMS_INCLUDE_DIRS} )

add_subdirectory( src )
add_subdirectory( python )
add_subdirectory( bin )
add_subdirectory( doc )

brainvisa_add_test(pyaimsalgo-resampling-tests
    "${PYTHON_EXECUTABLE_NAME}" -m soma.aimsalgo.tests.test_resampling)
brainvisa_add_test(pyaimsalgo-geometric-tests
    "${PYTHON_EXECUTABLE_NAME}" -m soma.aimsalgo.tests.test_geometric)
