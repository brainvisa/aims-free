TEMPLATE		= bundle
TARGET		    = aimstiff${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/tiffbundle.cc

LIBS	= ${LIBS_TIFF}
LIBS	+= -laimstiff${BUILDMODEEXT}
