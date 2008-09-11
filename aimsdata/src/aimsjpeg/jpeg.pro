TEMPLATE	= lib
TARGET		= aimsjpeg${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =			\
    io/jpegformat.h		\
    io/jpegformat_d.h		\
    io/jpegheader.h		\
    io/jpegR.h			\
    io/jpegW.h			\
    io/jpegfinderformat.h	\
    plugin/jpegplugin.h

SOURCES =			\
    io/jpegformat.cc		\
    io/jpegheader.cc		\
    plugin/jpegplugin.cc

LIBS	= ${LIBS_JPEG}
win32:LIBS	+= -lregex

