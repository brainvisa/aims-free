TEMPLATE        = lib
TARGET          = aimsgifti${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =               \
io/giftiformat.h        \
io/giftimeshformat_d.h  \
io/giftitextureformat_d.h  \
io/giftiheader.h        \
io/giftifinderformat.h  \
plugin/giftiplugin.h

SOURCES =               \
io/giftiformat.cc       \
io/giftiheader.cc       \
plugin/giftiplugin.cc

LIBS    = ${LIBS_GIFTI}

