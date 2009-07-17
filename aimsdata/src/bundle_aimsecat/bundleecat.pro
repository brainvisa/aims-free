TEMPLATE		= bundle
TARGET		    = aimsecat${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/ecatbundle.cc

LIBS	= ${LIBS_ECAT}
LIBS	+= -laimsecat${BUILDMODEEXT}
