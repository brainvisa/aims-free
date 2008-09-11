TARGET = AimsMeshParcellation2VolumeParcellation

#!include ../../config-app

release:LIBS	+= -laimsalgo
debug:LIBS	+= -laimsalgo-debug
irix:LIBS	+= -laimsalgo
darwin:LIBS	+= -laimsalgo

SOURCES = \
          main.cc
