TARGET = AimsCorrectionEpi

#!include ../../config-app

SOURCES = \
	  contextEPI.cc \
	  epiDistortion.cc \
	  g2dSmoother.cc \
	  g2dSmootherD.cc \
	  gCoefficients.cc \
	  gColumns.cc \
	  gLines.cc \
	  gRecursive.cc \
	  interpolation.cc \
          main.cc \
	  mutualInfo.cc \
	  pdf.cc

HEADERS = \
	  contextEPI.h \
	  epiDistortion.h \
	  g2dSmoother.h \
	  g2dSmootherD.h \
	  gCoefficients.h \
	  gColumns.h \
	  gLines.h \
	  gRecursive.h \
	  interpolation.h \
	  mutualInfo.h \
	  smoother.h
