
#!include ../../config-local

TEMPLATE = lib
TARGET = cartobase

LIBS += -L../../../lib -lcartobase_1 -lcartobase_0

SOURCES = \
  ../lib/config/info.cc \
  ../lib/getopt/getopt.cc \
  ../lib/object/areader.cc \
  ../lib/object/awriter.cc \
  ../lib/object/object_factory.cc \
  ../lib/object/object_map_string_vector.cc \
  ../lib/object/object_rcptr_map.cc \
  ../lib/object/object_rcptr_map_string_vector.cc \
  ../lib/object/object_set.cc \
  ../lib/object/pythonreader.cc \
  ../lib/object/pythonwriter.cc \
  ../lib/plugin/plugin.cc \
  ../lib/wip/io/datasourceinfo.cc \
  ../lib/wip/io/minfXML2.cc \
  ../lib/wip/io/pythonformatchecker.cc \
  ../lib/wip/io/pythonformatreader.cc \
  ../lib/wip/io/pythonformatwriter.cc \
  ../lib/wip/io/reader_object.cc \
  ../lib/wip/io/readeralgorithm.cc \
  ../lib/wip/io/writer.cc \
  ../lib/wip/io/xmlformatchecker.cc \
  ../lib/wip/io/xmlformatreader.cc

