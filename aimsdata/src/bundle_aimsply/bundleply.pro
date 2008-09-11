TEMPLATE	= bundle
TARGET		= aimsply${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/plybundle.cc

LIBS	+= -laimsply${BUILDMODEEXT} -laimsdata${BUILDMODEEXT}
