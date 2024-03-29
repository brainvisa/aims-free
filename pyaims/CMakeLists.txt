OPTION( AIMS_PARTIAL_PYAIMSSIP "Use only parts of PyAims, shorter to compile, for devel purpose" OFF )

set( PYAIMS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
include_directories( BEFORE ${PYAIMS_INCLUDE_DIRS} )

add_subdirectory( src )
add_subdirectory( python )
add_subdirectory( bin )
add_subdirectory( doc )

add_subdirectory( ../pyaimsalgo pyaimsalgo )

find_package( Doxygen )
IF ( DOXYGEN_FOUND )
    set( _doxygenInput ${PYAIMS_HEADERS} )
    set( DOXYFILE_PREDEFINED "${AIMS_DEFINITIONS}")
    set(aims_version "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}")
    set( DOXYFILE_TAGFILES "${CMAKE_BINARY_DIR}/share/doc/aimsdata-${aims_version}/doxygen/aimsdata.tag=../../aimsdata-${aims_version}/doxygen  ${CMAKE_BINARY_DIR}/share/doc/graph-${aims_version}/doxygen/graph.tag=../../graph-${aims_version}/doxygen  ${CMAKE_BINARY_DIR}/share/doc/cartobase-${aims_version}/doxygen/cartobase.tag=../../cartobase-${aims_version}/doxygen  ${CMAKE_BINARY_DIR}/share/doc/cartodata-${aims_version}/doxygen/cartodata.tag=../../cartodata-${aims_version}/doxygen")
    BRAINVISA_GENERATE_DOXYGEN_DOC( _doxygenInput
        INPUT_PREFIX "${CMAKE_BINARY_DIR}/include/pyaims"
        COMPONENT pyaims )
    add_dependencies( pyaims-doxygen aimsdata-doxygen graph-doxygen cartobase-doxygen cartodata-doxygen )
ENDIF ( DOXYGEN_FOUND )

#tests
BRAINVISA_COPY_FILES( ${PROJECT_NAME} "pyaims_tutorial.rst"
    "pyaims_tutorial_nb.ipynb" "volume_orientation.ipynb"
    SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx"
    DESTINATION "${PYTHON_INSTALL_DIRECTORY}/soma/aims/tests" )
brainvisa_add_test(pyaims-tests "${PYTHON_EXECUTABLE_NAME}" -m soma.aims.tests.test_pyaims)
brainvisa_add_test(pyaims-io-tests "${PYTHON_EXECUTABLE_NAME}" -m soma.aims.tests.test_pyaims_io)
brainvisa_add_test(pyaims-transform-tests "${PYTHON_EXECUTABLE_NAME}" -m soma.aims.tests.test_transform)
brainvisa_add_test(pyaims-transform-tests "${PYTHON_EXECUTABLE_NAME}" -m soma.aims.tests.test_volume_strides)
