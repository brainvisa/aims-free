TEMPLATE        = lib
TARGET		= graph${BUILDMODEEXT}

#!include ../../config


HEADERS=	config/graph_config.h		\
		doc/graph_doc.h			\
		graph/binary.h			\
		graph/dedge.h			\
		graph/edge.h			\
		graph/gfactory.h		\
		graph/gprinter.h		\
		graph/graph.h			\
		graph/graphobject.h		\
		graph/greader.h			\
		graph/gwriter.h			\
		graph/gwriter_python.h		\
		graph/uedge.h			\
		graph/vertex.h			\
		graph/gparser.h			\
		graph/gparser_xml.h		\
		graph/gparser_python.h		\
		graph/gparser_shfj.h		\
		io/gprinter.h			\
		io/greader.h			\
		io/gwriter.h			\
		io/treader.h			\
		io/twriter.h			\
		tree/base.h			\
		tree/tfactory.h			\
		tree/treader.h			\
                tree/tparser.h			\
                tree/tparser_shfj.h		\
                tree/tparser_python.h		\
		tree/tree.h			\
		tree/tvisitor.h			\
                tree/twriter.h

SOURCES = 	graph/dedge.cc				\
		graph/edge.cc				\
		graph/gfactory.cc			\
		graph/gprinter.cc			\
		graph/graph.cc				\
		graph/graphobject.cc			\
		graph/greader.cc			\
		graph/gwriter.cc			\
		graph/gwriter_python.cc			\
		graph/uedge.cc				\
		graph/vertex.cc				\
		graph/gparser.cc			\
		graph/gparser_xml.cc			\
		graph/gparser_python.cc			\
		graph/gparser_shfj.cc			\
		tree/base.cc				\
		tree/tfactory.cc			\
		tree/treader.cc				\
                tree/tparser.cc				\
                tree/tparser_shfj.cc			\
                tree/tparser_python.cc			\
		tree/tree.cc				\
		tree/tvisitor.cc			\
		tree/twriter.cc
