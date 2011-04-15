BRAINVISA_COPY_PYTHON_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME} )

find_package( Epydoc )
BRAINVISA_GENERATE_EPYDOC_DOC( "${CMAKE_BINARY_DIR}/python/soma/aimsalgo" "${CMAKE_BINARY_DIR}/python/soma/wip/aimsalgo"
     "share/doc/pyaimsalgo-${BRAINVISA_PACKAGE_VERSION_MAJOR}.${BRAINVISA_PACKAGE_VERSION_MINOR}/epydoc" )

find_package( Sphinx )
BRAINVISA_GENERATE_SPHINX_DOC( "../doc/sphinx"
  "share/doc/pyaimsalgo-${BRAINVISA_PACKAGE_VERSION_MAJOR}.${BRAINVISA_PACKAGE_VERSION_MINOR}/sphinx" TARGET pyaimsalgo-sphinx )

