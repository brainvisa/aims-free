TARGET          = AimsAttributedViewer


#!include ../../../config-app

#release-linux:LIBS = -L../../lib/linux $(LIBS_FOR_AIMSGUI)
#debug-linux:LIBS = -L../../lib/linux $(LIBS_FOR_AIMSGUI)
#release-solaris:LIBS = -L../../lib/solaris $(LIBS_FOR_AIMSGUI)
#debug-solaris:LIBS = -L../../lib/solaris $(LIBS_FOR_AIMSGUI)

LIBS	= $(LIBS_GUI)

release:LIBS += -laimsgui
debug:LIBS += -laimsgui-debug

HEADERS	= attributedViewer.h

SOURCES	= attributedViewer.cc


