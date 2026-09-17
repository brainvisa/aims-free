//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/gfactory.h>
#include <graph/graph/vertex.h>
#include <graph/graph/uedge.h>
#include <graph/graph/dedge.h>

using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphFactory::GraphFactory()
{
}


GraphFactory::~GraphFactory()
{
}


map<string, GraphFactory::Generator> & GraphFactory::_generators()
{
  static map<string, Generator> gen;
  return gen;
}


void GraphFactory::registerGenerator( const string & s, Generator g )
{
  _generators()[ s ] = g;
}


void GraphFactory::unregisterGenerator( const string & s )
{
  _generators().erase( s );
}


Vertex*
GraphFactory::makeVertex(const string& s)
{
  const map<string, Generator> & gen = _generators();
  map<string, Generator>::const_iterator	i = gen.find( s );
  if( i != gen.end() )
    return static_cast<Vertex *>( i->second( s ) );
  return new Vertex(s);
}


UEdge*
GraphFactory::makeUEdge(const string& s)
{
  const map<string, Generator> & gen = _generators();
  map<string, Generator>::const_iterator	i = gen.find( s );
  if( i != gen.end() )
    return static_cast<UEdge *>( i->second( s ) );
  return new UEdge(s);
}


DEdge*
GraphFactory::makeDEdge(const string& s)
{
  const map<string, Generator> & gen = _generators();
  map<string, Generator>::const_iterator	i = gen.find( s );
  if( i != gen.end() )
    return static_cast<DEdge *>( i->second( s ) );
  return new DEdge(s);
}
