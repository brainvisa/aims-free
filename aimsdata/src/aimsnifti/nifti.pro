TEMPLATE	= lib
TARGET		= aimsnifti${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =		    \
    io/niftiformat.h	    \
    io/niftiformat_d.h	    \
    io/niftiheader.h	    \
    io/niftiR.h		    \
    io/niftiR_d.h	    \
    io/niftiW.h		    \
    io/niftiW_d.h	    \
    io/znzlib/znzlib.h      \
    io/niftifinderformat.h  \
    plugin/niftiplugin.h    \
    io/niftilib/nifti1.h    \
    io/niftilib/nifti1_io.h

SOURCES =		    \
    io/niftiformat.cc	    \
    io/niftiheader.cc	    \
    io/niftiR.cc            \
    io/niftiW.cc            \
    io/niftilib/nifti1_io.c \
    io/znzlib/znzlib.c      \
    plugin/niftiplugin.cc

LIBS	= ${LIBS_NIFTI}

