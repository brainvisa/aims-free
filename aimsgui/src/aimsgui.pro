TEMPLATE=       subdirs

#!include ../config-local

SUBDIRS += lib \
    aimsqtformats

darwin:PSUBDIRS       += bundle_aimsqtformats

PSUBDIRS	+=	\
    aimsqsqlgraphformat \
    AimsLabelSelector \
    AimsAttributedViewer

