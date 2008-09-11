TARGET = AimsFileConvert

#!include ../../config-app

SOURCES = 	\
	fileConvert.cc
HEADERS = 

# workaround buggy lib linker on Darwin
# ugly, isn't it ?
#darwin:LIBS += ../aimsdata/io/fileFormat_volume.o  \
#    ../aimsdata/io/fileFormat_volumeVec.o ../aimsdata/io/fileFormat_bucket.o \
#    ../aimsdata/io/fileFormat_mesh.o ../aimsdata/io/fileFormat_texture.o \
#    ../aimsdata/io/spmR.o ../aimsdata/io/vidaR.o \
#    ../aimsdata/io/reader_d.o ../aimsdata/io/writer_d.o
