TEMPLATE	= lib
TARGET		= aimsecat${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =			\
    io/ecatformat.h		\
    io/ecatformat_d.h		\
    io/ecatheader.h		\
    io/ecatR.h			\
    io/ecatSinoheader.h		\
    io/ecatSinoR.h		\
    io/ecatSinoW.h		\
    io/ecatW.h			\
    io/ecatfinderformat.h	\
    plugin/ecatplugin.h		\
    sino/sino.h			\
    sino/sino_d.h

SOURCES =			\
    io/ecatformat.cc		\
    io/ecatheader.cc		\
    io/ecatR.cc			\
    io/ecatSinoheader.cc	\
    io/ecatSinoR.cc		\
    io/ecatSinoW.cc		\
    io/ecatW.cc			\
    plugin/ecatplugin.cc	\
    sino/sino.cc

LIBS	= ${LIBS_ECAT}
