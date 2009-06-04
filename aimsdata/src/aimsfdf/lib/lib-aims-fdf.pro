TEMPLATE	= lib
TARGET		= aimsfdf${BUILDMODEEXT}

#!include ../../../config

INCBDIR = aims

HEADERS =			\
    io/fdfformat.h		\
    io/fdfformat_d.h		\
    io/fdfheader.h		\
    io/fdfR.h			\
    io/fdffinderformat.h	\
    io/fdfutil.h     \
    plugin/fdfplugin.h

SOURCES =			\
    io/fdfformat.cc		\
    io/fdfheader.cc		\
    io/fdfutil.cc     \
    plugin/fdfplugin.cc

LIBS    = ${LIBS_FDF}

