TEMPLATE = lib
TARGET   = $(TARGETLIB)

#!include ../../config

darwin:LIBS      += -framework CoreServices
# linux:CXXFLAGS += -march=athlon-xp -mtune=athlon-xp

HEADERS =                           \
  doc/cartodata_doc.h               \
  io/carto2aimsheadertranslator.h   \
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
  wip/roi/doi.h                     \ 

HEADERS_NO_USE_SOMA_IO =            \
  io/gisformatchecker.h             \
  io/gisreader.h                    \
  io/gisreader_d.h                  \
  io/volumedatasource.h             \
  io/volumedatasource_d.h           \
  wip/io/bundleformatchecker.h      \
  wip/io/gisformatwriter.h          \
  wip/io/gisformatwriter_d.h        \
  wip/io/volumeformatreader.h       \
  wip/io/volumeformatreader_d.h     \

HEADERS_USE_SOMA_IO =               \
  io/volumeformatreader.h           \
  io/volumeformatreader_d.h         \
  io/volumeformatwriter.h           \
  io/volumeformatwriter_d.h         \
  io/volumeutilio.h                 \
  io/volumeutilio_d.h               \

SOURCES =                           \
  io/carto2aimsheadertranslator.cc  \
  volume/volume.cc                  \
  volume/volumeview.cc              \
  wip/roi/siteIterator.cc           \
  wip/roi/roi.cc                    \
  wip/roi/doi.cc                    \

SOURCES_NO_USE_SOMA_IO =            \
  io/gisreader.cc                   \
  io/gisformatchecker.cc            \
  io/reader_cartodata.cc            \
  io/volumedatasource.cc            \
  io/writer_cartodata.cc            \
  wip/io/bundleformatchecker.cc     \
  wip/io/gisformatwriter.cc         \
  wip/io/volumeformatreader.cc      \

SOURCES_USE_SOMA_IO =               \
  io/somaio_declare.cc              \
  config/plugins.cc                 \
  io/volumeutilio.cc                \


