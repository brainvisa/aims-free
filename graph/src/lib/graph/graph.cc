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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cstdlib>
#define AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
#include <graph/graph/graph.h>
#undef AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
#include <graph/graph/uedge.h>
#include <graph/graph/dedge.h>
#include <cartobase/object/syntax.h>
#include <cartobase/smart/rcptrtrick.h>
#include <soma-io/checker/achecker.h>
#include <vector>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

Graph::Graph(const string& s)
	: GraphObject(s), _factory(new GraphFactory)
{
}


Graph::Graph(const FactoryPtr factory, const string& s)
	: GraphObject(s), _factory(factory)
{
}


Graph::~Graph()
{
	clear();
}


void
Graph::clear()
{
	for (ESet::iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
          rc_ptr<Edge>  re(*e);
          // bidouille: dec the ref counter because we don't keep it in a
          // real rc_ptr
          --rc_ptr_trick::refCount(re);
          // now re will decrement it once more and eventually delete e.
          // delete *e;
	}
	_edges.clear();
	for (VSet::iterator v = _vertices.begin(); v != _vertices.end(); ++v)
	{
          rc_ptr<Vertex>  rv(*v);
          // bidouille: dec the ref counter because we don't keep it in a
          // real rc_ptr
          --rc_ptr_trick::refCount(rv);
          // now re will decrement it once more and eventually delete v.
          // delete *v;
	}
	_vertices.clear();
}


Vertex*
Graph::addVertex(const string& s)
{
	assert(_factory != 0);
	Vertex* v = _factory->makeVertex(s);
	_vertices.insert(v);
        rc_ptr<Vertex>  r(v);
        // bidouille: inc the ref counter because we don't keep it in a real
        //rc_ptr
        ++rc_ptr_trick::refCount(r);
	return v;
}


Vertex*
Graph::cloneVertex(const Vertex* vertex)
{
	// denis : j'allege ce test un peu dur 
	// assert(hasVertex(vertex) && vertex != 0);
	assert( vertex != 0 );

	Vertex	*v = vertex->cloneVertex();
	_vertices.insert(v);
        rc_ptr<Vertex>  r(v);
        // bidouille: inc the ref counter because we don't keep it in a real
        //rc_ptr
        ++rc_ptr_trick::refCount(r);
        return v;
}


bool
Graph::hasVertex(const Vertex* vertex) const
{
	return set<Vertex *>::const_iterator
	  ( _vertices.find(const_cast<Vertex*>(vertex)) ) != _vertices.end();
}


void
Graph::removeVertex(Vertex* vertex)
{
	if (!hasVertex(vertex))
	{
		throw range_error("Vertex not in Graph");
	}
	assert(vertex != 0);

	for (Vertex::iterator e = vertex->begin(); e != vertex->end(); ++e)
	{
		assert(hasEdge(*e));
		for (Edge::iterator v = (*e)->begin(); v != (*e)->end(); ++v)
		{
			assert(hasVertex(*v));
			if (*v != vertex)
			{
				(*v)->removeEdge(*e);
			}
		}
		_edges.erase(*e);
                rc_ptr<Edge>  re(*e);
                // bidouille: dec the ref counter because we don't keep it in a
                // real rc_ptr
                --rc_ptr_trick::refCount(re);
                // now re will decrement it once more and eventually delete e.
                //delete *e;
	}
	_vertices.erase(vertex);
        rc_ptr<Vertex>  rv(vertex);
        // bidouille: dec the ref counter because we don't keep it in a real
        // rc_ptr
        --rc_ptr_trick::refCount(rv);
        // now rv will decrement it once more and eventually delete vertex.
        // delete vertex;
}


Vertex*
Graph::randomVertex() const
{
	size_t n = _vertices.size();
	if (n == 0)
	{
		return 0;
	}
	// la ligne ci-dessous est fondamentalement boguée
	n = rand() % n;
	VSet::const_iterator v = _vertices.begin();
	for (size_t i = 0; i < n; ++i)
	{
		++v;
	}
	assert(*v != 0);
	return  *v;
}


set<Vertex*>
Graph::getVerticesWith(const string& s) const
{
	set<Vertex*> vertices;

	for (VSet::const_iterator v = begin(); v != end(); ++v)
	{
		if ((*v)->hasProperty(s))
		{
			vertices.insert(*v);
		}
	}

	return vertices;
}


Edge*
Graph::addDirectedEdge(Vertex* vertex1, Vertex* vertex2, string s)
{
	if (!hasVertex(vertex1) || !hasVertex(vertex2))
	{
		throw range_error("Vertex not in Graph");
	}
	assert(vertex1 != 0 && vertex2 != 0);
	assert(_factory != 0);

	Edge* edge = _factory->makeDEdge(s);

	edge->addVertices(vertex1, vertex2);
	vertex1->addEdge(edge);
	vertex2->addEdge(edge);

	_edges.insert(edge);
        rc_ptr<Edge>  r(edge);
        // bidouille: inc the ref counter because we don't keep it in a real
        //rc_ptr
        ++rc_ptr_trick::refCount(r);

	return edge;
}


Edge*
Graph::addUndirectedEdge(Vertex* vertex1, Vertex* vertex2, string s)
{
	if (!hasVertex(vertex1) || !hasVertex(vertex2))
	{
		throw range_error("Vertex not in Graph");
	}
	assert(vertex1 != 0 && vertex2 != 0);
	assert(_factory != 0);

	Edge* edge = _factory->makeUEdge(s);

	edge->addVertices(vertex1, vertex2);
	vertex1->addEdge(edge);
	vertex2->addEdge(edge);

	_edges.insert(edge);
        rc_ptr<Edge>  r(edge);
        // bidouille: inc the ref counter because we don't keep it in a real
        //rc_ptr
        ++rc_ptr_trick::refCount(r);

	return edge;
}


bool
Graph::hasEdge(const Edge* edge) const
{
	return set<Edge *>::const_iterator
	  ( _edges.find(const_cast<Edge*>(edge)) ) != _edges.end();
}


void
Graph::removeEdge(Edge* edge)
{
	if (_edges.find(edge) == _edges.end())
	{
		throw range_error("Edge not in graph");
	}
	assert(edge != 0);

	for (Edge::iterator v = edge->begin(); v != edge->end(); ++v)
	{
		assert((*v)->hasEdge(edge));
		(*v)->removeEdge(edge);
	}
	_edges.erase(edge);
        rc_ptr<Edge>  re(edge);
        // bidouille: dec the ref counter because we don't keep it in a
        // real rc_ptr
        --rc_ptr_trick::refCount(re);
        // now re will decrement it once more and eventually delete edge.
        // delete edge;
}


set<Edge*>
Graph::edges(const Vertex* vertex1, const Vertex* vertex2) const
{
	assert(hasVertex(vertex1) && hasVertex(vertex2));

	return vertex1->edgesTo(vertex2);
}


set<Edge*>
Graph::getEdgesWith(const string& s) const
{
	set<Edge*> edges;

	for (ESet::const_iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
		if ((*e)->hasProperty(s))
		{
			edges.insert(*e);
		}
	}

	return edges;
}


size_t
Graph::order() const
{
	return _vertices.size();
}


// TODO: remove this specialization
size_t
Graph::size() const
{
	return _edges.size();
}


size_t
Graph::edgesSize() const
{
	return _edges.size();
}


bool
Graph::isUndirected() const
{
	for (ESet::const_iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
		if ((*e)->isUndirected())
		{
			return false;
		}
	}
	return true;
}


bool
Graph::isDirected() const
{
	for (ESet::const_iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
		if ((*e)->isDirected())
		{
			return false;
		}
	}
	return true;
}


bool
Graph::check(const SyntaxSet& syntax, set<string>& missing) const
{
  AttributedChecker	ac( syntax );
  set<string>		m;

	// get the syntactic attribute
	SyntaxSet::const_iterator s 
          = syntax.find( getInterface<SyntaxedInterface>()->getSyntax() );
	assert(s != syntax.end());

	bool result = true;

	// make sure all mandatory semantic attributes are here
	set<string> attributes;
        Object	i;
        for( i=objectIterator(); i->isValid(); i->next() )
          attributes.insert( i->key() );
	for (SemanticSet::const_iterator i = s->second.begin(); 
             i != s->second.end(); ++i)
	{
		if (i->second.needed == true 
                    && attributes.find(i->first) == attributes.end())
		{
			missing.insert(i->first);
			result = false;
		}
	}

	// check vertices
	for (VSet::const_iterator v = _vertices.begin(); v != _vertices.end(); 
	     ++v)
	{
          m = ac.check( **v );
          if( !m.empty() )
            {
              result = false;
              missing.insert( m.begin(), m.end() );
            }
	}

	// check edges
	for (ESet::const_iterator e = _edges.begin(); e != _edges.end(); ++e)
	{
          m = ac.check( **e );
          if( !m.empty() )
            {
              result = false;
              missing.insert( m.begin(), m.end() );
            }
	}

	return result;
}


void
Graph::internalExtract(Graph& graph, const set<Vertex*>& vertices)
{
	map<Vertex*, Vertex*> translate;
	set<Edge*> edges;
	set<Vertex*>::const_iterator v;

	for (v = vertices.begin(); v != vertices.end(); ++v)
	{
		Vertex* vertex = (*v)->cloneVertex();
		graph._vertices.insert(vertex);
                rc_ptr<Vertex>  r(vertex);
                /* bidouille: inc the ref counter because we don't keep it in a
                real rc_ptr */
                ++rc_ptr_trick::refCount(r);
                translate[*v] = vertex;
	}
	for (v = vertices.begin(); v != vertices.end(); ++v)
	{
		for (Vertex::const_iterator e = (*v)->begin();
			e != (*v)->end(); ++e)
		{
			// existe deja ?
			if (edges.find(*e) == edges.end())
			{
				vector<Vertex*> table;
				Edge::const_iterator i = (*e)->begin();
				while (i != (*e)->end())
				{
					if (set<Vertex *>::const_iterator
					    ( vertices.find(*i) ) 
					    != vertices.end())
					{
						table.push_back(*i);
					}
					++i;
				}
				if (table.size() >= 2)
				{
					Edge* tmp = (*e)->cloneEdge();
					tmp->addVertices(
						translate[table[0]],
						translate[table[1]]);
					translate[table[0]]->addEdge(tmp);
					translate[table[1]]->addEdge(tmp);
					graph._edges.insert(tmp);
                                        rc_ptr<Edge>  r(tmp);
                                        /* bidouille: inc the ref counter
                                        because we don't keep it in a real
                                        rc_ptr */
                                        ++rc_ptr_trick::refCount(r);
                                        edges.insert(*e);
				}
			}
		}
	}
}


#include <cartobase/object/object_d.h>
INSTANTIATE_GENERIC_OBJECT_TYPE( Graph * )

