TEMPLATE	= lib
TARGET		= aimsply${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =			\
    io/plyformat.h		\
    io/plyformat_d.h		\
    io/plyheader.h		\
    io/plyr.h			\
    io/plyw.h			\
    io/plyfinderformat.h	\
    plugin/plyplugin.h		\
    rply/rply.h

SOURCES =			\
    io/plyformat.cc		\
    io/plyheader.cc		\
    plugin/plyplugin.cc		\
    rply/rply.c

LIBS	= ${LIBS_PLY}
