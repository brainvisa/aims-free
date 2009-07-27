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
    io/fdfprocpar.h	        \
    io/fdfprocpar_g.h	        \
    io/fdfutil.h                \
    plugin/fdfplugin.h

SOURCES =			\
    io/fdfformat.cc		\
    io/fdfheader.cc		\
    io/fdfprocpar.cc            \
    io/fdfutil.cc               \
    plugin/fdfplugin.cc

LIBS    = ${LIBS_FDF}

