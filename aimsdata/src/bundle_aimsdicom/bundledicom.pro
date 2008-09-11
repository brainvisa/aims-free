TEMPLATE		= bundle
default:TARGET		= aimsdicom
debug:TARGET		= aimsdicom-debug
insure:TARGET		= aimsdicom-insure
release:TARGET		= aimsdicom

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/dicombundle.cc

LIBS	= ${LIBS_DICOM}

default:LIBS	+= -laimsdicom
debug:LIBS	+= -laimsdicom-debug
insure:LIBS	+= -laimsdicom-insure
release:LIBS	+= -laimsdicom
