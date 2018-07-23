TEMPLATE = lib
TARGET = ecat${BUILDMODEEXT}

#!include ../../config-local

HEADERS = \
	kernel/analyze.h \
	kernel/convert_64.h \
	kernel/convert_70.h \
	kernel/ecat_model.h \
	kernel/interfile.h \
	kernel/isotope_info.h \
	kernel/machine_indep.h \
	kernel/matpkg.h \
	kernel/matrix.h \
	kernel/matrix_64.h \
	kernel/matrix_extra.h \
	kernel/num_sort.h \
	kernel/plandefs.h \
	kernel/sincos.h \
	lib_compress/compress.h \
	lib_compress/uvlc.h

SOURCES = \
	kernel/analyze.c \
	kernel/convert_64.c \
	kernel/convert_70.c \
	kernel/crash.c \
	kernel/ecat_model.c \
	kernel/interfile.c \
	kernel/load_volume7.c \
	kernel/machine_indep.c \
	kernel/make3dfilter.c \
	kernel/matpkg.c \
	kernel/matrix.c \
	kernel/matrix_64.c \
	kernel/matrix_extra.c \
	kernel/matrix_slice.c \
	kernel/num_sort.c \
	kernel/plandefs.c \
	kernel/save_volume7.c \
	kernel/sino_dets.c \
	lib_compress/c3101.c \
	lib_compress/c_uvlc.c \
	lib_compress/compress.c \
	lib_compress/d3101.c \
	lib_compress/d_uvlc.c \
	lib_compress/dclz.c \
	lib_compress/uvlc.c \
	lib_compress/z_matrix.c
