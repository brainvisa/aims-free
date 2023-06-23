
set( _headers
        init.h
        numpyarray.h
        sipnewtypedefs.h
        data/data.h
        data/sipdatatypes.h
        data/volumeoperators.h
        debugging/debuggingvol.h
        object/numconv.h
        object/object.h
        object/rcptrconv.h
        vector/numpyarrayfunc.h
        vector/numpytypes.h
        vector/numpytypes_d.h
        vector/sipset.h
        vector/sipstdvector.h
        vector/sipvector.h
        vector/stdvector.h
        vector/vector.h )

set( _sources
        init.cc
        debugging/debuggingvol.cc
        object/numconv.cc
        object/object.cc
        vector/numpyarrayfunc.cc
        vector/numpytypes.cc )

set(PYAIMS_HEADERS ${_headers} PARENT_SCOPE)
BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers pyaims headersTarget )
add_library( pyaims SHARED ${_sources} )
add_dependencies( pyaims ${headersTarget} )
set_property( TARGET pyaims PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
if( PYTHON_FLAGS )
    set_target_properties( pyaims PROPERTIES COMPILE_DEFINITIONS ${PYTHON_FLAGS})
endif()
target_link_libraries( pyaims aimsdata "${PYTHON_LIBRARY}" )

BRAINVISA_INSTALL( TARGETS pyaims
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )
