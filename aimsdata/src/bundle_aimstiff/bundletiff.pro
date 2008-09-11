TEMPLATE		= bundle
default:TARGET		= aimstiff
debug:TARGET		= aimstiff-debug
insure:TARGET		= aimstiff-insure
release:TARGET		= aimstiff

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/tiffbundle.cc

LIBS	= ${LIBS_TIFF}

default:LIBS	+= -laimstiff
debug:LIBS	+= -laimstiff-debug
insure:LIBS	+= -laimstiff-insure
release:LIBS	+= -laimstiff
