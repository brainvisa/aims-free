TEMPLATE	= lib
TARGET		= aimsqtformats${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

LIBS		= $(LIBS_FOR_AIMSGUI)

HEADERS =			\
    io/qtformats.h		\
    io/qtformats_d.h		\
    io/qtformatsheader.h	\
    io/qtformatsR.h		\
    io/qtformatsW.h		\
    io/qtfinderformats.h	\
    plugin/qtformatsplugin.h

SOURCES =			\
    io/qtformats.cc		\
    io/qtformatsheader.cc	\
    plugin/qtformatsplugin.cc

win32:LIBS	+= -lregex

