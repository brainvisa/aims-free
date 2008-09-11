TEMPLATE=       subdirs
SUBDIRS =       lib
PSUBDIRS = \
		counter_test	\
		extract_test	\
		gio_test	\
		graph_test	\
		syntax_test	\
		tio_test

# don't build commands with gcc2.95 on darwin
release-darwin:PSUBDIRS =
