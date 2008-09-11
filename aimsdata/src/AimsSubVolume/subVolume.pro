TARGET = AimsSubVolume

#!include ../../config-app

SOURCES = 	\
	subVolume.cc
HEADERS = 

# workaround buggy lib linker on Darwin
# ugly, isn't it ?
#darwin:LIBS += ../aimsdata/io/fileFormat_volume.o ../aimsdata/io/reader_d.o \
#    ../aimsdata/io/writer_d.o  \
#    ../aimsdata/io/spmR.o ../aimsdata/io/vidaR.o
