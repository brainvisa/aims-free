TEMPLATE        = bundle
TARGET          = aimsqsqlgraphformat${BUILDMODEEXT}

#!include ../../config-local

INCBDIR = aims

LIBS            = $(LIBS_PLUGINS)

HEADERS =                   \
io/qsqlgraphformat.h        \
plugin/qsqlgraphplugin.h

SOURCES =                       \
io/qsqlgraphformat.cc           \
plugin/qsqlgraphplugin.cc


