TEMPLATE		= bundle
TARGET	        = aimsqtformats${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/qtbundle.cc

LIBS	= ${LIBS_FOR_AIMSGUI}
LIBS	+= -laimsqtformats${BUILDMODEEXT}
