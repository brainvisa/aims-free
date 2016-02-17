TARGET = AimsRegisterFFDEstimate

#!include ../../../config-cpp-command

SOURCES = \
           lbfgsb.cc		    \
           l-bfgs-b_wrapper.f	 \
           l-bfgs-b.f	   \
           dataModel.cc    \
           ppdf.cc         \
           scaleControl.cc \
           locdef.cc

#HEADERS = \
#           dataModel.h     \
#           ffd.h           \
#           ppdf.h          \
#           scaleControl.h  \
#           fortran.h       \
#           f77char.h       \
#           f77cmplx.h      \
#           f77matrx.h      \
#           lbfgsb.h 
			  
LIBS = ${LIBS_FORTRAN}
