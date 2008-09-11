TEMPLATE	= lib
TARGET		= aimsminc${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims
#darwin:LFLAGS_SHLIB	= -bundle
#darwin:LFLAGS_SONAME	=
#darwin:EXTENSION_SHLIB	= .so

HEADERS =			\
    io/mincfinderformat.h	\
    io/mincformat.h		\
    io/mincformat_d.h		\
    io/mincheader.h		\
    io/mincR.h			\
    io/mincW.h			\
    plugin/mincplugin.h

SOURCES =			\
    io/mincformat.cc		\
    io/mincheader.cc		\
    io/mincW.cc		        \
    plugin/mincplugin.cc

LIBS	= ${LIBS_MINC}
