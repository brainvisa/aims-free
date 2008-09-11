TEMPLATE		= bundle
default:TARGET		= aimsvida
debug:TARGET		= aimsvida-debug
insure:TARGET		= aimsvida-insure
release:TARGET		= aimsvida

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/vidabundle.cc

LIBS	= ${LIBS_VIDA}

default:LIBS	+= -laimsvida
debug:LIBS	+= -laimsvida-debug
insure:LIBS	+= -laimsvida-insure
release:LIBS	+= -laimsvida
