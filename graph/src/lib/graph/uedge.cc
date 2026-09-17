//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/uedge.h>

using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

UEdge::UEdge(string s)
  : Edge(s)
{
}


UEdge::UEdge(const UEdge& x)
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
UEdge::cloneEdge() const
{
	return new UEdge(*this);
}


UEdge::~UEdge()
{
}


bool
UEdge::isDirected() const
{
	return false;
}


bool
UEdge::isUndirected() const
{
	return true;
}
