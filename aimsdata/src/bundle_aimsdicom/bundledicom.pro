TEMPLATE		= bundle
TARGET		    = aimsdicom${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/dicombundle.cc

LIBS	= ${LIBS_DICOM}
LIBS	+= -laimsdicom${BUILDMODEEXT}
