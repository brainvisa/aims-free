//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/graphobject.h>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphObject::GraphObject(const string& s)
	: AttributedObject(s)
{
}


GraphObject::GraphObject(const GraphObject& x)
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
    AttributedObject(x)
{
}


GraphObject::~GraphObject()
{
}
