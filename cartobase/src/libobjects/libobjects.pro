TEMPLATE = lib
TARGET = cartobase${BUILDMODEEXT}

#!include ../../config

LIBS += -lcartobase_0

SOURCES = \
  ../lib/object/object_map_string_vector.cc \
  ../lib/object/object_rcptr_map.cc \
  ../lib/object/object_rcptr_map_string_vector.cc \
  ../lib/object/object_set.cc

