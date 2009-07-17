TEMPLATE		= bundle
TARGET		    = aimsjpeg${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

SOURCES =			\
    bundle/jpegbundle.cc

LIBS	= ${LIBS_JPEG}
LIBS	+= -laimsjpeg${BUILDMODEEXT}
