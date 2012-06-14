TEMPLATE        = lib
TARGET		= aimsgui${BUILDMODEEXT}

#!include ../../config-local

INCBDIR = aims

HEADERS = selection/labelselector.h \
    selection/labelselector_p.h \
    listview/editablelistviewitem.h \
    listview/orderinglist.h \
    listview/qalistview.h \
    listview/qpython.h \
    attributedview/attributedview.h \
    graph/qsqlgraphdatabase.h \
    graph/qsqlgraphformatheader.h \
    graph/qsqlgraphhelper.h \
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

SOURCES = graph/qsqlgraphdatabase.cc \
    graph/qsqlgraphformatheader.cc \
    graph/qsqlgraphhelper.cc \
    qtcompat/qtoolbutton.cc \
    selection/labelselector.cc \
    listview/editablelistviewitem.cc \
    listview/orderinglist.cc \
    listview/qalistview.cc \
    listview/qpython.cc \
    attributedview/attributedview.cc \
    plugin/plugin.cc \
    qtmisc/qwidgetobject.cc \
    qtmisc/resolvesymlinks.cc

