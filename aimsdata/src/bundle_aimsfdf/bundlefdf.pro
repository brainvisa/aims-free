TEMPLATE		= bundle
default:TARGET		= aimsfdf
debug:TARGET		= aimsfdf-debug
insure:TARGET		= aimsfdf-insure
release:TARGET		= aimsfdf

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/fdfbundle.cc

LIBS = $(LIBS_FDF) 

default:LIBS	+= -laimsfdf
debug:LIBS	+= -laimsfdf-debug
insure:LIBS	+= -laimsfdf-insure
release:LIBS	+= -laimsfdf
