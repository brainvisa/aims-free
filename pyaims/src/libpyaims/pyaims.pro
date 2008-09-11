TEMPLATE        = lib
TARGET  	= pyaims${BUILDMODEEXT}

#!include ../../config-local

# LIBS = ${AIMS_LIBS}

HEADERS = \
        init.h \
        numpyarray.h \
        data/data.h \
        data/sipdatatypes.h \
        io/reader.h \
        io/writer.h \
        object/object.h \
        object/rcptrconv.h \
        vector/numpyarrayfunc.h \
        vector/sipvector.h \
        vector/stdvector.h \
        vector/vector.h 

SOURCES = \
        init.cc \
        object/object.cc \
        vector/numpyarrayfunc.cc
