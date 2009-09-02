TEMPLATE = lib
#!include ../../config

TARGET = cartobase_1

LIBS += -L../../../lib -lcartobase_0

SOURCES = \
  ../lib/object/object_map.cc \
  ../lib/object/object_vector.cc

