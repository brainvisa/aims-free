TARGET = AimsMIRegister

# include library config instead of command config to avoind linking
# against libbioprocessing: AimsMIRegister doesn't need it
#!include ../../config-app

SOURCES = \
          miProbe.cc    \
          main.cc

HEADERS = \
          miProbe.h      \
          corratiofunc.h \
          mutualfunc.h   \
          objfunc.h
