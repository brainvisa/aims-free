TEMPLATE        = bundle
TARGET          = aimsgifti${BUILDMODEEXT}

#!include ../../config

INCBDIR = aims

HEADERS =                  \
io/gifticlib/gifti_tool.h  \
io/gifticlib/gifti_io.h    \
io/gifticlib/gifti_xml.h   \
io/giftiformat.h           \
io/giftimeshformat_d.h     \
io/giftitextureformat_d.h  \
io/giftiheader.h           \
io/giftifinderformat.h     \
io/giftiutil.h             \
plugin/giftiplugin.h

SOURCES =                 \
io/gifticlib/gifti_tool.c \
io/gifticlib/gifti_io.c   \
io/gifticlib/gifti_xml.c  \
io/giftiformat.cc         \
io/giftiheader.cc         \
io/giftiutil.cc           \
plugin/giftiplugin.cc     \

LIBS    = ${LIBS_GIFTI}

