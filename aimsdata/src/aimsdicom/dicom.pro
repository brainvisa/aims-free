TEMPLATE	= lib
TARGET		= aimsdicom${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =			\
    io/dicomformat.h		\
    io/dicomformat_d.h		\
    io/dicomheader.h		\
    io/dicomR.h			\
    io/dicomW.h			\
    io/dicomfinderformat.h	\
    plugin/dicomplugin.h

SOURCES =			\
    io/dicomformat.cc		\
    io/dicomheader.cc		\
    plugin/dicomplugin.cc

LIBS	= ${LIBS_DICOM}
