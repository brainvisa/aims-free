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

#ifndef PYAIMS_GRAPH_GRAPH_H
#define PYAIMS_GRAPH_GRAPH_H

#ifndef PYAIMS_RCPTR_Graph_DEFINED
#define PYAIMS_RCPTR_Graph_DEFINED
#include <cartobase/smart/rcptr.h>
class Graph;
typedef carto::rc_ptr<Graph> rc_ptr_Graph;
#endif

inline PyObject* pyaimsConvertFrom_GraphP( void * a )
{
  return sipConvertFromType( a, sipFindType( "Graph" ), 0 );
}


inline void* pyaimsConvertTo_GraphP( PyObject * o )
{
  int isErr = 0;
  return sipConvertToType( o, sipFindType( "Graph" ), 0, 0, 0, &isErr );
}


inline int pyaimsGraphP_Check( PyObject* o )
{
  return sipCanConvertToType( o, sipFindType( "Graph" ),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

// ---

inline PyObject* pyaimsConvertFrom_GraphR( void * a )
{
  return sipConvertFromType( a, sipFindType( "rc_ptr_Graph" ), 0 );
}


inline void* pyaimsConvertTo_GraphR( PyObject * o )
{
  int isErr = 0;
  return sipConvertToType( o, sipFindType( "rc_ptr_Graph" ), 0, 0, 0, &isErr );
}


inline int pyaimsGraphR_Check( PyObject* o )
{
  return sipCanConvertToType( o, sipFindType( "rc_ptr_Graph" ),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}

// ---

inline PyObject* pyaimsConvertFrom_VertexP( void * a )
{
  return sipConvertFromType( a, sipFindType( "Vertex" ), 0 );
}


inline void* pyaimsConvertTo_VertexP( PyObject * o )
{
  int isErr = 0;
  return sipConvertToType( o, sipFindType( "Vertex" ), 0, 0, 0, &isErr );
}


inline int pyaimsVertexP_Check( PyObject* o )
{
  return sipCanConvertToType( o, sipFindType( "Vertex" ),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}


inline PyObject* pyaimsConvertFrom_EdgeP( void * a )
{
  return sipConvertFromType( a, sipFindType( "Edge" ), 0 );
}


inline void* pyaimsConvertTo_EdgeP( PyObject * o )
{
  int isErr = 0;
  return sipConvertToType( o, sipFindType( "Edge" ), 0, 0, 0, &isErr );
}


inline int pyaimsEdgeP_Check( PyObject* o )
{
  return sipCanConvertToType( o, sipFindType( "Edge" ),
                              SIP_NOT_NONE | SIP_NO_CONVERTORS );
}


#endif

