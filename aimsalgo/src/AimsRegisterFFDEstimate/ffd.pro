TARGET = AimsRegisterFFDEstimate

#!include ../../../config-cpp-command

SOURCES = \
           lbfgsb.cc		    \
           dataModel.cc    \
           ppdf.cc         \
           scaleControl.cc \
           locdef.cc

#HEADERS = \
#           dataModel.h     \
#           ffd.h           \
#           ppdf.h          \
#           scaleControl.h  \
#           lbfgsb.h

LIBS =