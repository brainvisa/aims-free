#!include ../../config-local

TEMPLATE = sip
TARGET   = aimsguisip

LIBBDIR = python/soma/aims

LIBS += -laimsgui${BUILDMODEEXT}

SIPS = aimsgui.sip

