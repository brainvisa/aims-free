TEMPLATE	= lib
TARGET		= aimstiff${BUILDMODEEXT}

#!include ../../../config

INCBDIR = aims

HEADERS =			\
    io/tiffformat.h		\
    io/tiffformat_d.h		\
    io/tiffheader.h		\
    io/tiffR.h			\
    io/tiffW.h			\
    io/tifffinderformat.h	\
    plugin/tiffplugin.h

SOURCES =			\
    io/tiffformat.cc		\
    io/tiffheader.cc		\
    plugin/tiffplugin.cc

LIBS	= ${LIBS_TIFF}
