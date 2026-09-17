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

