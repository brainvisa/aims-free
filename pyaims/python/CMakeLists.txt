BRAINVISA_COPY_PYTHON_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_NAME} )

# find_package( Epydoc )
# BRAINVISA_GENERATE_EPYDOC_DOC(
#   "${CMAKE_BINARY_DIR}/python/soma/aims"
#   "${CMAKE_BINARY_DIR}/python/soma/wip/aims"
#   "share/doc/pyaims-${BRAINVISA_PACKAGE_VERSION_MAJOR}.${BRAINVISA_PACKAGE_VERSION_MINOR}/epydoc"
#   EXCLUDE soma.signature numpy.* pickle StringIO copy* math soma.uuid soma.wip* string sys types scipy
# )

if( BRAINVISA_BVMAKER )
  find_package( Sphinx )
else()
  set(_target TARGET soma-python-sphinx )
endif()
BRAINVISA_GENERATE_SPHINX_DOC( "../doc/sphinx"
  "share/doc/pyaims-${BRAINVISA_PACKAGE_VERSION_MAJOR}.${BRAINVISA_PACKAGE_VERSION_MINOR}/sphinx"
  ${_target} )
