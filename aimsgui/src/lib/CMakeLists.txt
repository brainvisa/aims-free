
set( _headers
    selection/labelselector.h
    selection/labelselector_p.h
    listview/orderinglist.h
    listview/qatreewidget.h
    listview/qpython.h
    attributedview/attributedview.h
    graph/qsqlgraphdatabase.h
    graph/qsqlgraphformatheader.h
    graph/qsqlgraphhelper.h
    plugin/aimsguiplugin.h
    qtcompat/qtoolbutton.h
    qtmisc/initqapplication.h
    qtmisc/resolvesymlinks.h
)

set( _sources
    graph/qsqlgraphdatabase.cc
    graph/qsqlgraphformatheader.cc
    graph/qsqlgraphhelper.cc
    qtcompat/qtoolbutton.cc
    selection/labelselector.cc
    listview/orderinglist.cc
    listview/qatreewidget.cc
    listview/qpython.cc
    attributedview/attributedview.cc
    plugin/plugin.cc
    qtmisc/initqapplication.cc
    qtmisc/qwidgetobject.cc
    qtmisc/resolvesymlinks.cc
)

BRAINVISA_COPY_AND_INSTALL_HEADERS( _headers aims headersTarget )
set(AIMSGUI_HEADERS ${_headers} PARENT_SCOPE)
BRAINVISA_ADD_MOC_FILES( _sources ${_headers} )
add_library( aimsgui SHARED ${_sources} )
add_dependencies( aimsgui ${headersTarget} )
set_property( TARGET aimsgui PROPERTY VERSION ${${PROJECT_NAME}_VERSION} )
target_link_libraries( aimsgui ${QT_LIBRARIES} aimsdata )
BRAINVISA_INSTALL( TARGETS aimsgui
              DESTINATION lib
              COMPONENT ${PROJECT_NAME} )

