
set( _headers
        init.h
        numpyarray.h
        data/data.h
        data/sipdatatypes.h
        data/volumeoperators.h
        debugging/debuggingvol.h
        io/reader.h
        io/writer.h
        object/numconv.h
        object/object.h
        object/rcptrconv.h
        vector/numpyarrayfunc.h
        vector/sipset.h
        vector/sipvector.h
        vector/stdvector.h
        vector/vector.h )

set( _sources
        init.cc
        debugging/debuggingvol.cc
        object/numconv.cc
        object/object.cc
        vector/numpyarrayfunc.cc )

set(PYAIMS_HEADERS ${_headers} PARENT_SCOPE)
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers pyaims headersTarget )
add_library( pyaims SHARED ${_sources} )
add_dependencies( pyaims ${headersTarget} )
set_property( TARGET pyaims PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( pyaims aimsdata "${PYTHON_LIBRARY}" )
BRAINVISA_INSTALL( TARGETS pyaims
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
