TEMPLATE		= bundle
TARGET		    = aimsqtformats${BUILDMODEEXT}

#!include ../../config-local

INCBDIR = aims

SOURCES =			\
    bundle/qtbundle.cc

LIBS	= -laimsqtformats${BUILDMODEEXT}
LIBS    += $(LIBS_AIMSQTFORMATS)

