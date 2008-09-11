TEMPLATE	= bundle
TARGET		= aimsminc${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/mincbundle.cc

LIBS	= ${LIBS_MINC}

LIBS	+= -laimsminc${BUILDMODEEXT}
