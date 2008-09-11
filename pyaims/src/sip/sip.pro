TEMPLATE        = sip
TARGET		= aimssip

LIBBDIR = python/soma/aims

#!include ../../config-local

LIBS = ${SIP_LIBS}

SIPS = aims.sip

HEADERS = graph.h \
  info.h
