TEMPLATE		= bundle
TARGET		    = aimsvida${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/vidabundle.cc

LIBS	= ${LIBS_VIDA}
LIBS	+= -laimsvida${BUILDMODEEXT}
