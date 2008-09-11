TEMPLATE		= bundle
default:TARGET		= aimsjpeg
debug:TARGET		= aimsjpeg-debug
insure:TARGET		= aimsjpeg-insure
release:TARGET		= aimsjpeg

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/jpegbundle.cc

LIBS	= ${LIBS_JPEG}

default:LIBS	+= -laimsjpeg
debug:LIBS	+= -laimsjpeg-debug
insure:LIBS	+= -laimsjpeg-insure
release:LIBS	+= -laimsjpeg
