TEMPLATE		= bundle
default:TARGET		= aimsqtformats
debug:TARGET		= aimsqtformats-debug
insure:TARGET		= aimsqtformats-insure
release:TARGET		= aimsqtformats

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/qtbundle.cc

LIBS	= ${LIBS_FOR_AIMSGUI}
LIBS	+= -laimsqtformats${BUILDMODEEXT}
