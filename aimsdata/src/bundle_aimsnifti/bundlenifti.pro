TEMPLATE	= bundle
TARGET		= aimsnifti${BUILDMODEEXT}

#!include ../../config

INCBDIR = 

SOURCES =			\
    bundle/niftibundle.cc

LIBS	+= -laimsnifti${BUILDMODEEXT} -laimsdata${BUILDMODEEXT}
