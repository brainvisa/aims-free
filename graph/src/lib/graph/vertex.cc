//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================
#include <cstdlib>
#include <cstdlib>
#define AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
#include <graph/graph/vertex.h>
#undef AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
#include <graph/graph/edge.h>

using namespace carto;
using namespace std;

//=============================================================================
//	M E T H O D S
//=============================================================================

Vertex::Vertex(string s)
	: GraphObject(s)
{
}


Vertex::Vertex(const Vertex& x)
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
	// On copie seulement ce qui concerne le GraphObject et
	// est son ascendant AttributedObject.
	// Par contre, les relations dans _edges ne sont pas copiees !
	// En effet, on veut copier les attributs d'un sommet,
	// pas la topologie du graphe...
}


Vertex *
Vertex::cloneVertex() const
{
	return new Vertex(*this);
}


Vertex::~Vertex()
{
}


size_t
Vertex::edgesSize() const
{
	return _edges.size();
}


size_t
Vertex::size() const
{
	return _edges.size();
}


bool
Vertex::hasEdge(const Edge* edge) const
{
	return set<Edge *>::const_iterator
	  ( _edges.find(const_cast<Edge*>(edge)) ) != _edges.end();
}


set<Edge*>
Vertex::edgesTo(const Vertex* vertex) const
{
	set<Edge*> edges;

	for (Vertex::const_iterator e = vertex->begin(); e != vertex->end(); ++e)
	{
		assert(*e != 0);
		bool hit = false;
		for (Edge::iterator v = (*e)->begin(); v != (*e)->end(); ++v)
		{
			assert(*v != 0);
			if (*v == vertex)
			{
				if ((*e)->isDirected() && hit)
				{
					edges.insert(*e);
					break;
				}
				hit = true;
			}
			else if (*v == this)
			{
				hit = true;
				if ((*e)->isUndirected())
				{
					edges.insert(*e);
					break;
				}
			}
		}
	}

	return edges;
}


void
Vertex::addEdge(Edge* edge)
{
	assert(edge != 0 && _edges.find(edge) == _edges.end());
	_edges.insert(edge);
}


void
Vertex::removeEdge(Edge* edge)
{
	assert(_edges.find(edge) != _edges.end());
	_edges.erase(edge);
}


set<Vertex*>
Vertex::inNeighbours() const
{
	set<Vertex*> neighbourhood;
	for (ESet::const_iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
		assert(*e != 0);
		for (Edge::iterator v = (*e)->begin(); v != (*e)->end(); ++v)
		{
			if (*v != this)
			{
				assert(*v != 0);
				neighbourhood.insert(*v);
			}
			else if ((*e)->isDirected())
			{
				break;
			}
		}
	}
	return neighbourhood;
}


set<Vertex*>
Vertex::outNeighbours() const
{
	set<Vertex*> neighbourhood;
	for (ESet::const_iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
		assert(*e != 0);
		for (Edge::reverse_iterator v = (*e)->rbegin();
			v != (*e)->rend(); ++v)
		{
			if (*v != this)
			{
				assert(*v != 0);
				neighbourhood.insert(*v);
			}
			else if ((*e)->isDirected())
			{
				break;
			}
		}
	}
	return neighbourhood;
}


Vertex*
Vertex::randomNeighbour() const
{
	set<Vertex*> neighbourhood = outNeighbours();
	size_t n = neighbourhood.size();
	if (n == 0)
	{
		return 0;
	}
	// la ligne ci-dessous est fondamentalement boguée
	n = rand() % n;
	set<Vertex*>::iterator v = neighbourhood.begin();
	for (size_t i = 0; i < n; ++i)
	{
		++v;
	}
	assert(*v != 0);
	return  *v;
}


#include <cartobase/object/object_d.h>
INSTANTIATE_GENERIC_OBJECT_TYPE( Vertex * )

