TEMPLATE = lib
TARGET   = $(TARGETLIB)

#!include ../../config

darwin:LIBS      += -framework CoreServices
# linux:CXXFLAGS += -march=athlon-xp -mtune=athlon-xp

HEADERS =                           \
  doc/cartodata_doc.h               \
  io/carto2aimsheadertranslator.h   \
  io/volumeformatreader.h           \
  io/volumeformatreader_d.h         \
  io/volumeformatwriter.h           \
  io/volumeformatwriter_d.h         \
  io/volumeutilio.h                 \
  io/volumeutilio_d.h               \
  volume/volume.h                   \
  volume/volume_d.h                 \
  volume/volumeoperators.h          \
  volume/volumeproxy.h              \
  volume/volumeproxy_d.h            \
  volume/volumeutil.h               \
  volume/volumeutil_d.h             \
  volume/volumeview.h               \
  wip/roi/siteIterator.h            \
  wip/roi/roi.h                     \
  wip/roi/doi.h

SOURCES =                           \
  config/plugins.cc                 \
  io/carto2aimsheadertranslator.cc  \
  io/somaio_declare.cc              \
  io/volumeutilio.cc                \
  volume/volume.cc                  \
  wip/roi/siteIterator.cc           \
  wip/roi/roi.cc                    \
  wip/roi/doi.cc

