TEMPLATE        = lib
TARGET  	= pyaims${BUILDMODEEXT}

#!include ../../config-local

# LIBS = ${AIMS_LIBS}

HEADERS = \
        init.h \
        numpyarray.h \
        data/data.h \
        data/sipdatatypes.h \
        debugging/debuggingvol.h \
        io/reader.h \
        io/writer.h \
        object/numconv.h \
        object/object.h \
        object/rcptrconv.h \
        vector/numpyarrayfunc.h \
        vector/sipset.h \
        vector/sipvector.h \
        vector/stdvector.h \
        vector/vector.h 

SOURCES = \
        init.cc \
        debugging/debuggingvol.cc \
        object/numconv.cc \
        object/object.cc \
        vector/numpyarrayfunc.cc
