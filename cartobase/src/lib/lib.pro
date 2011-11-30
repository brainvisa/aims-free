TEMPLATE = lib
#TARGET = $(TARGETLIB)
TARGET = cartobase_0

#!include ../../config

darwin:LIBS += -framework CoreServices

HEADERS = \
  algorithm/algorithm.h \
  allocator/allocator.h \
  allocator/mappingcopy.h \
  allocator/mappingro.h \
  allocator/mappingrw.h \
  config/cartobase_config.h \
  config/info.h \
  config/paths.h \
  config/verbose.h \
#   config/version.h \
  containers/allocatedvector.h \
  containers/block.h \
  datasource/bufferdatasource.h \
  datasource/chaindatasource.h \
  datasource/cstreamdatasource.h \
  datasource/datasource.h \
  datasource/fddatasource.h \
  datasource/filedatasource.h \
  datasource/filteringdatasource.h \
  datasource/streamdatasource.h \
  doc/cartobase_doc.h \
  doc/getopt_doc.h \
  doc/io_doc.h \
  doc/object_doc.h \
  exception/assert.h \
  exception/errno.h \
  exception/file.h \
  exception/format.h \
  exception/ioexcept.h \
  exception/parse.h \
  getopt/getopt.h \
  object/achecker.h \
  object/areader.h \
  object/attributed.h \
  object/awriter.h \
  object/headered.h \
  object/lexicon.h \
  object/object.h \
  object/object_d.h \
  object/object_factory.h \
  object/object_internal.h \
  object/property.h \
  object/propertyfilter.h \
  object/pythonreader.h \
  object/pythonreader_d.h \
  object/pythonwriter.h \
  object/pythonwriter_d.h \
  object/sprinter.h \
  object/sreader.h \
  object/swriter.h \
  object/syntax.h \
  object/syntobject.h \
  object/syntobject_d.h \
  pattern/observable.h \
  pattern/observer.h \
  plugin/plugin.h \
  sigc++/cartosigcmacro.h \
  sigc++/cartosigcmacroend.h \
  sigc++/sigc++.h \
  sigc++/signal.h \
  sigc++/trackable.h \
  smart/mutexrcptr.h \
  smart/rcobject.h \
  smart/rcptr.h \
  smart/rcptrtrick.h \
  smart/scopedptr.h \
  smart/sharedptr.h \
  smart/smartdoc.h \
  smart/weakobject.h \
  smart/weakptr.h \
  stream/attachable_cuifstream.h \
  stream/counter.h \
  stream/cuifstream.h \
  stream/directory.h \
  stream/fdinhibitor.h \
  stream/fdstream.h \
  stream/fileutil.h \
  stream/fistreambuf.h \
  stream/fostreambuf.h \
  stream/sstream.h \
  stream/uncomment.h \
  thread/cpu.h \
  thread/exclusiveContext.h \
  thread/loopContext.h \
  thread/loopThread.h \
  thread/mutex.h \
  thread/mutexprivatepthread.h \
  thread/mutexprivatewindows.h \
  thread/semaphore.h \
  thread/threadCancel.h \
  thread/threadCondition.h \
  thread/threadedLoop.h \
  thread/threadGauge.h \
  thread/thread_g.h \
  thread/thread.h \
  type/byte_order.h \
  type/conversion.h \
  type/converter_d.h \
  type/converter.h \
  type/limits.h \
  type/limits_gcc3.h \
  type/select.h \
  type/string_conversion.h \
  type/types.h \
  type/typetraits.h \
  uuid/uuid.h \
  wip/io/asciidatasourcetraits.h \
  wip/io/creator.h \
  wip/io/datasourceinfo.h \
  wip/io/formatchecker.h \
  wip/io/formatdictionary.h \
  wip/io/formatdictionary_d.h \
  wip/io/formatreader.h \
  wip/io/formatreader_d.h \
  wip/io/formatwriter.h \
  wip/io/headertranslator.h \
  wip/io/itemreader.h \
  wip/io/itemwriter.h \
  wip/io/minfXML2.h \
  wip/io/pythonformatchecker.h \
  wip/io/pythonformatreader.h \
  wip/io/pythonformatwriter.h \
  wip/io/reader.h \
  wip/io/reader_d.h \
  wip/io/readeralgorithm.h \
  wip/io/writer.h \
  wip/io/writer_d.h \
  wip/io/xmlformatchecker.h \
  wip/io/xmlformatreader.h \
  zoolib/atomiccount.h \
  zoolib/ZAtomic.h \
  zoolib/zconfig.h \
  zoolib/zconfigd.h \
  zoolib/zconfigtest.h \
  zoolib/ZDebug.h

SOURCES = \
  algorithm/algorithm.cc \
  allocator/allocator.cc \
  allocator/mappingcopy.cc \
  allocator/mappingro.cc \
  allocator/mappingrw.cc \
  allocator/meminfo.cc \
#  config/info.cc \
  config/paths.cc \
  config/verbose.cc \
  config/version.cc \
  datasource/bufferdatasource.cc \
  datasource/chaindatasource.cc \
  datasource/cstreamdatasource.cc \
  datasource/datasource.cc \
  datasource/fddatasource.cc \
  datasource/filedatasource.cc \
  datasource/filteringdatasource.cc \
  datasource/streamdatasource.cc \
  exception/assert.cc \
  exception/errno.cc \
  exception/file.cc \
  exception/format.cc \
  exception/ioexcept.cc \
#  getopt/getopt.cc \
  object/achecker.cc \
#  object/areader.cc \
#  object/awriter.cc \
  object/headered.cc \
  object/object.cc \
#  object/object_factory.cc \
#  object/object_map.cc \
#  object/object_map_string_vector.cc \
#  object/object_rcptr_map.cc \
#  object/object_rcptr_map_string_vector.cc \
#  object/object_set.cc \
  object/object_usual.cc \
#  object/object_vector.cc \
  object/property.cc \
  object/propertyfilter.cc \
#  object/pythonreader.cc \
#  object/pythonwriter.cc \
  object/sprinter.cc \
  object/sreader.cc \
  object/swriter.cc \
  object/syntobject.cc \
  pattern/observer.cc \
#  plugin/plugin.cc \
  smart/mutexrcptr.cc \
  smart/rcobject.cc \
  smart/sharedptr.cc \
  smart/weakobject.cc \
  stream/attachable_cuifstream.cc \
  stream/counter.cc \
  stream/cuifstream.cc \
  stream/directory.cc \
  stream/fdinhibitor.cc \
  stream/fdstream.cc \
  stream/fileutil.cc \
  stream/fistreambuf.cc \
  stream/fostreambuf.cc \
  stream/uncomment.cc \
  thread/cpu.cc \
  thread/exclusiveContext.cc \
  thread/loopContext.cc \
  thread/loopThread.cc \
  thread/mutex.cc \
  thread/semaphore.cc \
  thread/threadCancel.cc \
  thread/threadCondition.cc \
  thread/threadedLoop.cc \
  thread/threadGauge.cc \
  thread/thread.cc \
  type/byte_order.cc \
  type/converter.cc \
  type/string_conversion.cc \
  uuid/uuid.cc \
  wip/io/asciidatasourcetraits.cc \
#  wip/io/datasourceinfo.cc \
  wip/io/formatdictionary.cc \
#  wip/io/minfXML2.cc \
#  wip/io/pythonformatchecker.cc \
#  wip/io/pythonformatreader.cc \
#  wip/io/pythonformatwriter.cc \
#  wip/io/reader_object.cc \
#  wip/io/readeralgorithm.cc \
#  wip/io/writer.cc \
#  wip/io/xmlformatchecker.cc \
#  wip/io/xmlformatreader.cc

