add_subdirectory( libpyaims )
add_subdirectory( sip )
set(PYAIMS_HEADERS ${PYAIMS_HEADERS} ${PYAIMSSIP_HEADERS} PARENT_SCOPE)
