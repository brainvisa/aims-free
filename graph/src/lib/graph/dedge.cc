//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/dedge.h>

using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

DEdge::DEdge(string s)
	: Edge(s)
{
}


DEdge::DEdge(const DEdge& x)
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
    Edge(x)
{
}


Edge *
DEdge::cloneEdge() const
{
	return new DEdge(*this);
}


DEdge::~DEdge()
{
}


bool
DEdge::isDirected() const
{
	return true;
}


bool
DEdge::isUndirected() const
{
	return false;
}
