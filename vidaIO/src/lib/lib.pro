TEMPLATE	= lib
TARGET		= vidaIO${BUILDMODEEXT}

#!include ../../config

HEADERS = \
	kernel/analyzeDb.h \
	kernel/shfjData.h \
	kernel/vidaio.h

SOURCES = \
	kernel/close.c \
	kernel/consigne.c \
	kernel/lowIoVhdr.c \
	kernel/lowIoVimg.c \
	kernel/lowIoVinfo.c \
	kernel/lowUtil.c \
	kernel/open.c \
	kernel/read.c \
	kernel/util.c \
	kernel/write.c
