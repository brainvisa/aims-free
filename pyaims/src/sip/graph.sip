class GraphObject : carto::GenericObject
{
%TypeHeaderCode
#include <graph/graph/graphobject.h>
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_GraphObject,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:
    virtual ~GraphObject();

protected :
  GraphObject(const GraphObject &);
  GraphObject(const std::string &);

public:
    virtual std::string type() const /AutoGen/;
    virtual size_t size() const /AutoGen/;
    virtual bool isScalar() const;
    virtual double getScalar() const /AutoGen/;
    virtual bool isString() const;
    virtual std::string getString() const /AutoGen/;
    virtual bool isNone() const;
    virtual bool isArray() const /AutoGen/;
    virtual bool isDynArray() const /AutoGen/;
    virtual bool isDictionary() const /AutoGen/;
    virtual bool isIterable() const /AutoGen/;
    /*virtual*/ carto::Object objectIterator() const /AutoGen/;
    virtual bool hasProperty( const std::string & ) const
      /PyName=has_key, AutoGen/;
    virtual bool isIterator() const /AutoGen/;
    virtual bool isDictionaryIterator() const /AutoGen/;
    virtual bool isValid() const /AutoGen/;
    /*virtual*/ carto::Object currentValue() const  /Factory, AutoGen/;
    virtual std::string key() const  /Factory, AutoGen/;
    virtual long intKey() const /AutoGen/;

private:
    virtual carto::GenericObject* clone() const /AutoGen/;

};

class Vertex : GraphObject
{
%TypeHeaderCode
#include <graph/graph/vertex.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipEdge.h"
#endif
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_Vertex,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:
  unsigned edgesSize() const;

  Vertex *randomNeighbour() const;
  SIP_PYTUPLE neighbours() const;
%MethodCode
  std::set<Vertex *>		s = sipCpp->neighbours();
  std::set<Vertex *>::iterator	it = s.begin(), et = s.end();

  PyObject *l = PyTuple_New( s.size() );
  int			c = 0;
  for( ; it != et; ++it, ++c )
#if SIP_VERSION >= 0x040400
    PyTuple_SetItem( l, c, sipConvertFromInstance( *it, sipClass_Vertex, 0 ) );
#else
    PyTuple_SetItem( l, c, sipMapCppToSelfSubClass( *it, sipClass_Vertex ) );
#endif
  sipRes = l;
%End


  SIP_PYTUPLE edges() const;
%MethodCode
  PyObject *l = PyTuple_New( sipCpp->edgesSize() );
  Vertex::const_iterator i, e = sipCpp->end();
  int			c = 0;
  for( i=sipCpp->begin(); i!=e; ++i, ++c )
#if SIP_VERSION >= 0x040400
    PyTuple_SetItem( l, c, sipConvertFromInstance( *i, sipClass_Edge, 0 ) );
#else
    PyTuple_SetItem( l, c, sipMapCppToSelfSubClass( *i, sipClass_Edge ) );
#endif
  sipRes = l;
%End

private:
  Vertex( const Vertex & );
};


class Edge : GraphObject
{
%TypeHeaderCode
#include <graph/graph/edge.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipVertex.h"
#endif
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_Edge,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:
  bool hasVertex( const Vertex * ) const;
  unsigned verticesSize() const;

  SIP_PYTUPLE vertices() const;
%MethodCode
  PyObject *l = PyTuple_New( sipCpp->verticesSize() );
  Edge::const_iterator i, e = sipCpp->end();
  int			c = 0;
  for( i=sipCpp->begin(); i!=e; ++i, ++c )
#if SIP_VERSION >= 0x040400
    PyTuple_SetItem( l, c, sipConvertFromInstance( *i, sipClass_Vertex, 0 ) );
#else
    PyTuple_SetItem( l, c, sipMapCppToSelfSubClass( *i, sipClass_Vertex ) );
#endif
  sipRes = l;
%End

private:
  Edge( const Edge & );
};


class Graph : GraphObject
{
%TypeHeaderCode
#include <graph/graph/graph.h>
#if SIP_VERSION < 0x040700
#include "sipaimssipEdge.h"
#endif
%End

%TypeCode
#include <aims/io/aimsGraphR.h>
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_Graph,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:
  Graph( const std::string & = "" );
  virtual ~Graph();

  // virtual bool isIterable() const ;
  void clear();
  unsigned order() const;
  unsigned edgesSize() const;
  bool isUndirected() const;
  bool isDirected() const;
  Vertex *addVertex( const std::string & = "" );
  Edge* addEdge( Vertex *vertex1, Vertex *vertex2, std::string s="" );
  Edge* addDirectedEdge( Vertex *vertex1, Vertex *vertex2, std::string s="" );
  Vertex *cloneVertex( const Vertex * );
  bool hasVertex( const Vertex * ) const;
  void removeVertex( Vertex * );
  void removeEdge( Edge *edge );
  Vertex *randomVertex();
  virtual size_t size() const;
%MethodCode
  sipRes = sipCpp->carto::AttributedObject::size();
%End

  const set_VertexPtr & vertices() const /NoCopy/;
  const set_EdgePtr & edges() const /NoCopy/;

  void extract( Graph &, SIP_PYOBJECT );
%MethodCode
  PyObject  *iter = PyObject_GetIter( a1 );
  if( !iter )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "argument 2 is not iterable" );
  }
  else
  {
    std::set<Vertex *> vert;
    Vertex    *v;
    PyObject  *p;
    while( ( p = PyIter_Next( iter ) ) )
    {
      if( sipCanConvertToInstance( p, sipClass_Vertex, SIP_NOT_NONE ) )
      {
        v = static_cast<Vertex *>(
          sipConvertToInstance( p, sipClass_Vertex, 0, SIP_NOT_NONE, 0,
            &sipIsErr ) );
        vert.insert( v );
      }
      else
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_TypeError,
          "argument 2 has elements which are not Vertex objects" );
        Py_DECREF( p );
        break;
      }
      Py_DECREF( p );
    }
    Py_DECREF( iter );
    if( PyErr_Occurred() )
      sipIsErr = 1;
    if( !sipIsErr )
    {
      sipCpp->extract( *a0, vert.begin(), vert.end() );
    }
  }
%End

  void loadAllMissingElements();
%MethodCode
  aims::AimsGraphReader::loadAllMissingElements( *sipCpp );
%End

private:
  Graph( const Graph & );

};

