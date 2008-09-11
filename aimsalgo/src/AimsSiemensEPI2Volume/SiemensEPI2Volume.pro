TARGET = AimsSiemensEPI2Volume

#!include ../../config-app

SOURCES = 	\
	main.cc
HEADERS = 

LIBS = ${LIBS_DICOM}
irix:LIBS += -lgen
