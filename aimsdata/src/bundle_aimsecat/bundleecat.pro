TEMPLATE		= bundle
default:TARGET		= aimsecat
debug:TARGET		= aimsecat-debug
insure:TARGET		= aimsecat-insure
release:TARGET		= aimsecat

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/ecatbundle.cc

LIBS	= ${LIBS_ECAT}

default:LIBS	+= -laimsecat
debug:LIBS	+= -laimsecat-debug
insure:LIBS	+= -laimsecat-insure
release:LIBS	+= -laimsecat
