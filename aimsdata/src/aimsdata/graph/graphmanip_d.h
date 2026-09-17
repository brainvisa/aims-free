#ifndef AIMS_GRAPH_GRAPHMANIP_D_H
#define AIMS_GRAPH_GRAPHMANIP_D_H

#include <aims/graph/graphmanip.h>
#include <aims/io/datatypecode.h>
#include <aims/io/finder.h>
#include <graph/graph/graph.h>

namespace aims
{

  template<typename T>
  void GraphManip::storeAims( Graph & g, GraphObject* vertex, 
                              const std::string & att, carto::rc_ptr<T> obj )
  {
    vertex->setProperty( att, obj );
    carto::rc_ptr<GraphElementTable>	mgec;
    if( !g.getProperty( "aims_objects_table", mgec ) )
      {
        mgec = carto::rc_ptr<GraphElementTable>( new GraphElementTable );
        g.setProperty( "aims_objects_table", mgec );
      }
    GraphElementCode	& gec 
      = graphElementCodeByAtt( g, vertex->getSyntax(), att );
    carto::DataTypeCode<T>	dtc;
    if( gec.objectType != dtc.objectType() || gec.dataType != dtc.dataType() )
    {
      gec.objectType = dtc.objectType();
      gec.dataType = dtc.dataType();
      gec.global_filename = att
        + defaultExtensionForObjectType( gec.objectType, gec.dataType );
    }
    gec.attribute = att;
    std::string att2 = att;
    if( att2.substr( 0, 5 ) == "aims_" )
      att2 = att2.substr( 5, att2.length() - 5 );
    if( gec.global_index_attribute.empty() )
      gec.global_index_attribute = att2 + "_label";
    if( gec.local_file_attribute.empty() )
      gec.local_file_attribute = att2 + "_filename"
      + GraphManip::defaultExtensionForObjectType( gec.objectType,
                                                   gec.dataType );
  }

}

#endif
