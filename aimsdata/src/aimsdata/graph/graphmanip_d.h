/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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
    gec.objectType = dtc.objectType();
    gec.dataType = dtc.dataType();
    gec.attribute = att;
    std::string att2 = att;
    if( att2.substr( 0, 5 ) == "aims_" )
      att2 = att2.substr( 5, att2.length() - 5 );
    if( gec.global_index_attribute.empty() )
      gec.global_index_attribute = att2 + "_label";
    if( gec.local_file_attribute.empty() )
      gec.local_file_attribute = att2 + "_filename";
  }

}

#endif
