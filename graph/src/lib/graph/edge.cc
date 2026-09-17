//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/edge.h>
#include <graph/graph/vertex.h>
#include <graph/graph/graph.h>

using namespace carto;
using namespace std;

//=============================================================================
//	M E T H O D S
//=============================================================================

Edge::Edge(string s)
	: GraphObject(s)
{
	_vertices[0] = 0;
	_vertices[1] = 0;
}


Edge::Edge(const Edge& x)
  : 
    RCObject(), 
    Interface(), 
    StringInterface(), 
    ScalarInterface(), 
    SizeInterface(), 
    IterableInterface(), 
    ArrayInterface(), 
    DynArrayInterface(), 
    DictionaryInterface(), 
    IteratorInterface(), 
    DictionaryIteratorInterface(), 
    GraphObject(x)
{
	_vertices[0] = 0;
	_vertices[1] = 0;
}


Edge::~Edge()
{
}


void
Edge::addVertices(Vertex* vertex1, Vertex* vertex2)
{
	_vertices[0] = vertex1;
	_vertices[1] = vertex2;
	assert(vertex1 != 0 && vertex2 != 0);
}


void
Edge::removeVertex(Vertex* vertex)
{
	assert(hasVertex(vertex));

	if (_vertices[0] == vertex)
	{
		_vertices[0] = 0;
	}
	if (_vertices[1] == vertex)
	{
		_vertices[1] = 0;
	}
}


size_t Edge::verticesSize() const
{
  return _vertices.size();
}


#include <cartobase/object/object_d.h>
INSTANTIATE_GENERIC_OBJECT_TYPE( Edge * )

