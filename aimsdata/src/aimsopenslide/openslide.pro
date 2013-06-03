TEMPLATE	= lib
TARGET		= aimsopenslide${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS = \
    io/osformatchecker.h \
    io/osimagereader.h \
    io/osimagereader_d.h \
    plugin/osplugin.h

SOURCES = \
    io/osformatchecker.cc \
    io/osimagereader.cc \
    plugin/osplugin.cc

LIBS	= 
