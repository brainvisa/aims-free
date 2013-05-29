TEMPLATE	= lib
TARGET		= aimsgis${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS = \
    io/gisformatchecker.h \
    io/gisimagereader.h \
    io/gisimagereader_d.h \
    io/gisimagewriter.h	\
    io/gisimagewriter_d.h \
    plugin/gisplugin.h

SOURCES = \
    io/gisformatchecker.cc \
    io/gisimagereader.cc \
    io/gisimagewriter.cc \
    plugin/gisplugin.cc

LIBS	= 
