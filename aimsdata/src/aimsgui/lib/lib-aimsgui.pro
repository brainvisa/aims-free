TEMPLATE        = lib
TARGET		= aimsgui${BUILDMODEEXT}

#!include ../../../config

INCBDIR = aims

LIBS		= $(LIBS_FOR_AIMSGUI)


HEADERS = selection/labelselector.h \
    selection/labelselector_p.h \
    listview/editablelistviewitem.h \
    listview/orderinglist.h \
    listview/qalistview.h \
    listview/qpython.h \
    plugin/aimsguiplugin.h \
    qtcompat/qaccel.h \
    qtcompat/qbutton.h \
    qtcompat/qbuttongroup.h \
    qtcompat/qdragobject.h \
    qtcompat/qfiledialog.h \
    qtcompat/qgrid.h \
    qtcompat/qgroupbox.h \
    qtcompat/qhbox.h \
    qtcompat/qhbuttongroup.h \
    qtcompat/qheader.h \
    qtcompat/qhgroupbox.h \
    qtcompat/qimageio.h \
    qtcompat/qkeyevent.h \
    qtcompat/qlistbox.h \
    qtcompat/qlistview.h \
    qtcompat/qmouseevent.h \
    qtcompat/qpopupmenu.h \
    qtcompat/qserversocket.h \
    qtcompat/qtoolbutton.h \
    qtcompat/qvaluelist.h \
    qtcompat/qvbox.h \
    qtcompat/qvbuttongroup.h \
    qtcompat/qvgroupbox.h \
    qtcompat/qwidgetlist.h \
    qtmisc/resolvesymlinks.h

SOURCES = qtcompat/qtoolbutton.cc \
    selection/labelselector.cc \
    listview/editablelistviewitem.cc \
    listview/orderinglist.cc \
    listview/qalistview.cc \
    listview/qpython.cc \
    plugin/plugin.cc \
    qtmisc/resolvesymlinks.cc

