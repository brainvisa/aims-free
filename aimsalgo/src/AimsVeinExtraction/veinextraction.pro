TARGET = AimsVeinExtraction

#!include ../../config-app

SOURCES = volume/volume.cc \
	  geodesicminimalpath/geodesicminimalpath.cc \
          main.cc

HEADERS = volume/volume.h \
	  geodesicminimalpath/geodesicminimalpath.h \
          segmentation/veinextraction.h \
	  segmentation/veinextraction_d.h
  
          
