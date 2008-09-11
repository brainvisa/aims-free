TEMPLATE        = lib
TARGET		= ecat+${BUILDMODEEXT}

#!include ../../config

HEADERS = \
	io/io.h \
	io/rwToolBox.h \
	io/general.h

SOURCES = \
	io/rwToolBox.c \
	io/ecatOpen.c \
	io/ecatRead.c \
	io/ecatSinoRead.c \
	io/ecatSinoOpen.c \
	io/ecatWrite.c \
	io/ecatSinoWrite.c
