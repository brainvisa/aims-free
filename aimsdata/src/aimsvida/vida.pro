TEMPLATE	= lib
TARGET		= aimsvida${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims
#darwin:LFLAGS_SHLIB	= -bundle
#darwin:LFLAGS_SONAME	=
#darwin:EXTENSION_SHLIB	= .so

HEADERS =			\
    io/vidafinderformat.h	\
    io/vidaformat.h		\
    io/vidaformat_d.h		\
    io/vidaheader.h		\
    io/vidaR.h			\
    io/vidaW.h			\
    plugin/vidaplugin.h

SOURCES =			\
    io/vidaformat.cc		\
    io/vidaheader.cc		\
    io/vidaR.cc			\
    io/vidaW.cc			\
    plugin/vidaplugin.cc

LIBS	= ${LIBS_VIDA}
