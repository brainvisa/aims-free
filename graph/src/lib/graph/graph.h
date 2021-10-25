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

#ifndef GRAPH_GRAPH_GRAPH_H
#define GRAPH_GRAPH_GRAPH_H


//=============================================================================
//    H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#ifndef GRAPH_GRAPH_GRAPHOBJECT_H
#include <graph/graph/graphobject.h>
#endif
#ifndef GRAPH_GRAPH_VERTEX_H
#include <graph/graph/vertex.h>
#endif
#ifndef GRAPH_GRAPH_EDGE_H
#include <graph/graph/edge.h>
#endif
#ifndef GRAPH_GRAPH_GFACTORY_H
#include <graph/graph/gfactory.h>
#endif
#include <cartobase/smart/rcptr.h>
#include <cartobase/type/types.h>

#include <algorithm>
#include <functional>
#include <set>
#include <stdexcept>


//=============================================================================
//    C L A S S  D E C L A R A T I O N
//=============================================================================

/**    The base class for graphs.
    It contains and manages Vertex and Edge objects.
*/
class GRAPH_API Graph : public GraphObject
{

public:

    typedef std::set<Vertex*> VSet;
    typedef std::set<Edge*> ESet;

    typedef carto::rc_ptr<GraphFactory>   FactoryPtr;
    typedef VSet::value_type             value_type;
#ifndef _WIN32
    ///    pointer is absent from MS Visual C++ / Intel Win32
    typedef VSet::pointer                pointer;
#endif
    typedef VSet::reference              reference;
    typedef VSet::const_reference        const_reference;
    typedef VSet::iterator               iterator;
    typedef VSet::const_iterator         const_iterator;
    typedef VSet::reverse_iterator       reverse_iterator;
    typedef VSet::const_reverse_iterator const_reverse_iterator;

    /**    \param s syntactic attribute of the new graph
    */
    Graph(const std::string& s = "");

    /**    \param factory abstract factory used instead of the
        default factory
        \param s syntactic attribute of the new graph
    */
    Graph(const FactoryPtr factory, const std::string& s = "");

    /**    The destructor is responsible for releasing the memory
        allocated by the addVertex and addEdge methods.
        The programmer should \b never delete vertex and
        edge objects; they are managed by the graph.
    */
    virtual ~Graph();

    /**    Extract a subgraph.
        \param graph extracted graph
        \param iv1 iterator on the first vertex to extract
        \param iv2 iterator on the last vertex to extract
        \exception range_error if a vertex is not in the graph
    */
    template <class InputIterator>
    void extract(Graph& graph, InputIterator iv1, InputIterator iv2);

    //---------------------------------------------------------------------
    /**    @name Clean up*/
    //---------------------------------------------------------------------
    //@{

    /**    Delete vertices and edges
    */
    void clear();

    //@}

    //---------------------------------------------------------------------
    /**    @name Vertex handling*/
    //---------------------------------------------------------------------
    //@{

    /**    Allocate and add a vertex to the graph. Note that the
        pointer returned by this method is to be seen as a
        simple identifier for the new vertex; the programmer
        should \b never delete it. It is the responsability
        of the graph which creates the vertex to delete it.
        \return a pointer to the allocated vertex
        \param s syntactic attribute of the new vertex
    */
    Vertex* addVertex(const std::string& s = "");

    /**    Clone a vertex of the graph, \b without attaching
        it to any edge. This a commodity method for copying
        the attributes, not the topology of the graph! Note that the
        pointer returned by this method is to be seen as a
        simple identifier for the new vertex; the programmer
        should \b never delete it. It is the responsability
        of the graph which creates the vertex to delete it.
        \param vertex pointer to the vertex to clone
        \return a clone of the given vertex
    */
    Vertex* cloneVertex(const Vertex* vertex);

    /**    Does the graph contain a given vertex?
        \return true if the graph contains it, false otherwise
        \param vertex pointer to the vertex to check
    */
    bool hasVertex(const Vertex* vertex) const;

    /**    Delete and remove a vertex from the graph. The pointer
        becomes invalid and should \b not be used
        afterwards.
        \param vertex pointer to the vertex to remove
        \exception range_error if vertex is not in the graph
    */
    void removeVertex(Vertex* vertex);

    /**    Return a random vertex (CAUTION: not perfectly random!)
        \return a random vertex
    */
    Vertex* randomVertex() const;

    /**    Return the vertices of the graph
        \return the internal collection of vertex pointers
    */
    const std::set<Vertex*>& vertices() const;

    /**    Find the vertices which contain a given semantic attribute
        \param s semantic attribute to look for
        \return the set of vertices with this semantic attribute
    */
    std::set<Vertex*> getVerticesWith(const std::string& s) const;

    /**    Find the vertices which contain a given
        semantic attribute/value pair
        \param s semantic attribute to look for
        \param t value this semantic attribute must match
        \return the set of vertices that have the right pair
    */
    template <class T>
    std::set<Vertex*> getVerticesWith(const std::string& s,
                      const T& t) const;

    //@}

    //---------------------------------------------------------------------
    /**    @name Edge handling*/
    //---------------------------------------------------------------------
    //@{

    /**    Allocate and add to the graph an undirected edge
        containing the given vertices. Note that the pointer
        returned by this method is to be seen as a simple
        identifier for the new edge; the programmer should
        \b never delete it. It is the responsability of the
        graph which creates the edge to delete it.
        \return a pointer to the allocated edge
        \param vertex1 first vertex
        \param vertex2 second vertex
        \param s syntactic attribute of the new edge
        \exception range_error if a vertex is not in the graph
    */
    Edge* addUndirectedEdge(Vertex* vertex1, Vertex* vertex2, std::
                string s);

    /**    A synonym of addUnDirectedEdge - see above
    */
    Edge* addEdge(Vertex* vertex1, Vertex* vertex2, std::string s = "");

    /**    Allocate and add to the graph a directed edge
        containing the given vertices. Note that the pointer
        returned by this method is to be seen as a simple
        identifier for the new edge; the programmer should
        \b never delete it. It is the responsability of the
        graph which creates the edge to delete it.
        \return a pointer to the allocated edge
        \param vertex1 first vertex
        \param vertex2 second vertex
        \param s syntactic attribute of the new edge
        \exception range_error if a vertex is not in the graph
    */
    Edge* addDirectedEdge(Vertex* vertex1, Vertex* vertex2, std::string s);

    /**    Does the graph contain a particular edge?
        \return true if the graph contains it, false otherwise
        \param  edge pointer to the edge to check
    */
    bool hasEdge(const Edge* edge) const;

    /**    Delete and remove an edge from the graph. The pointer
        becomes invalid and should \b not be used
        afterwards.
        \param edge pointer to the edge to remove
        \exception range_error if \c edge is not in the graph
    */
    void removeEdge(Edge* edge);

    /**    Return the edges of the graph
        \return the internal collection of edge pointers
    */
    const std::set<Edge*>& edges() const;

    /**    Return the set of edges linking two given vertices
        \param vertex1 is the source vertex
        \param vertex2 is the sink vertex
        \return the edges linking the source to the sink
    */
    std::set<Edge*> edges(const Vertex* vertex1,
                  const Vertex* vertex2) const;

    /**    Find the edges which contain a given semantic attribute
        \param  s semantic attribute to look for
        \return the set of edges with this semantic attribute
    */
    std::set<Edge*> getEdgesWith(const std::string& s) const;

    /**    Find the edges which contain a given pair
        semantic attribute/value
        \param  s semantic attribute to look for
        \param  t value this semantic attribute must match
        \return the set of edges that have the right pair
        attribute/value
    */
    template <class T>
    std::set<Edge*> getEdgesWith(const std::string& s, const T& t) const;

    //@}

    //---------------------------------------------------------------------
    /**    @name Iterators iterate on the vertices*/
    //---------------------------------------------------------------------
    //@{

    /**    Get the beginning of the vertex collection of the graph
        \return an iterator pointing to the beginning of the
        vertex collection
    */
    iterator begin();

    /**    Get the end of the vertex collection of the graph
        \return an iterator pointing to the end of the
        vertex collection
    */
    iterator end();

    /**    Get the beginning of the vertex collection of the graph
        \return a const_iterator pointing to the beginning of
        the vertex collection
    */
    const_iterator begin() const;

    /**    Get the end of the vertex collection of the graph
        \return a const_iterator pointing to the end of the
        vertex collection
    */
    const_iterator end() const;

    /**    Get the beginning of the reversed vertex collection of
        the graph
        \return a reverse_iterator pointing to the beginning
        of the reversed vertex collection
    */
    reverse_iterator rbegin();

    /**    Get the end of the reversed vertex collection of
        the graph
        \return a reverse_iterator pointing to the end of the
        reversed vertex collection
    */
    reverse_iterator rend();

    /**    Get the beginning of the reversed vertex collection of
        the graph
        \return a const_reverse_iterator pointing to the
        beginning of the reversed vertex collection
    */
    const_reverse_iterator rbegin() const;

    /**    Get the end of the reversed vertex collection of
        the graph
        \return a const_reverse_iterator pointing to the end
        of the reversed vertex collection
    */
    const_reverse_iterator rend() const;

    //@}

    //---------------------------------------------------------------------
    /**    @name Property querying methods*/
    //---------------------------------------------------------------------
    //@{

    /**    The order of a graph is the number of its vertices
        \return number of vertices in the graph
    */
    size_t order() const;

    /** \deprecated{specialization deprecated because ambiguous: use
        edgesSize() for the number of edges. In a future release, size()
        will return the number properties as it does in GenericObject} */
    size_t size() const
      __attribute__((__deprecated__("use edgeSize() for "
        "the number of edges. In a future release, size() will return the "
        "number properties as it does in GenericObject")));
    /**    The edgesSize of a graph is the number of its edges
        \return number of edges in the graph
    */
    size_t edgesSize() const;

    /**    Does the graph contain only undirected edges?
        \return tue if all the vertices are undirected,
        false otherwise
    */
    virtual bool isUndirected() const;

    /**    Does the graph contain only directed edges?
        \return true if all the vertices are directed,
        true otherwise
    */
    virtual bool isDirected() const;

    /**    Reimplemented to check children recursively.
        \param    syntax  syntax to check against
        \param    missing completed with the names of missing attributes
        \return    true if the object is valid, false otherwise
    */
    virtual bool check(const carto::SyntaxSet& syntax,
               std::set<std::string>& missing) const;

    //@}

private:

    //---------------------------------------------------------------------
    /**    @name Subgraph extraction*/
    //---------------------------------------------------------------------
    //@{

    /**    Extract a subgraph.
        \param vertices vertices to extract
        \param graph extracted graph
        \exception range_error if a vertex is not in the graph
    */
    void internalExtract(Graph& graph, const std::set<Vertex*>& vertices);

    //@}

    //---------------------------------------------------------------------
    /**    @name Disable copy*/
    //---------------------------------------------------------------------
    //@{

    /**    Copy constructor
    */
    Graph(const Graph&);

    /**    Assignment operator
    */
    Graph& operator=(const Graph&);

    //@}

    //---------------------------------------------------------------------
    /**    @name Data*/
    //---------------------------------------------------------------------
    //@{

    /**    Collection of vertices created and managed by the graph.
    */
    std::set<Vertex*> _vertices;

    /**    Collection of edges created and managed by the graph.

        \b Note: edges are presently ordered using the
        default comparison operators, which perform a mere
        comparison of the pointers themselves. For efficiency,
        an alternative ordering based on a comparison of the
        sequence of vertices contained in an edge can be
        implemented if edges are to be queried based on such
        a vertex sequence (see LINK Programmer's Manual and
        source code). A multiset would then have to be used
        instead of a set as different edges can be equal if
        they contain the same vertex sequence.
    */
    std::set<Edge*> _edges;

    //@}

protected:
    /**    Abstract factory used to create vertices and edges
    */
    FactoryPtr _factory;

};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template <> class DataTypeCode< Graph >
  {
  public:
    static std::string objectType()
    { return "Graph"; }
    static std::string dataType()
    { return "VOID"; }
    static std::string name() 
    { 
      return "Graph"; 
    }
  };

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


//=============================================================================
//    I N L I N E  M E T H O D S
//=============================================================================

inline
const std::set<Vertex*>&
Graph::vertices() const
{
    return _vertices;
}


template <class T>
inline
std::set<Vertex*>
Graph::getVerticesWith(const std::string& s, const T& t) const
{
    std::set<Vertex*> vertices;

    for (VSet::const_iterator v = begin(); v != end(); ++v)
    {
        if ((*v)->hasProperty(s))
        {
            T tmp;
            if ((*v)->getProperty(s, tmp) && tmp == t)
            {
                vertices.insert(*v);
            }
        }
    }

    return vertices;
}


inline
Edge*
Graph::addEdge(Vertex* vertex1, Vertex* vertex2, std::string s) 
{
    return addUndirectedEdge(vertex1, vertex2, s);
}


inline
const std::set<Edge*>&
Graph::edges() const
{
    return _edges;
}


template <class T>
inline
std::set<Edge*>
Graph::getEdgesWith(const std::string& s, const T& t) const
{
    std::set<Edge*> edges;

    for (ESet::iterator e = _edges.begin(); e != _edges.end(); ++e)
    {
        if ((*e)->hasProperty(s))
        {
            T tmp;
            if ((*e)->getProperty(s, tmp) && tmp == t)
            {
                edges.insert(*e);
            }
        }
    }

    return edges;
}


template <class InputIterator>
inline
void
Graph::extract(Graph& graph, InputIterator iv1, InputIterator iv2)
{
    internalExtract(graph, std::set<Vertex*>(iv1, iv2));
}


inline
Graph::iterator
Graph::begin()
{
    return _vertices.begin();
}


inline
Graph::iterator
Graph::end()
{
    return _vertices.end();
}


inline
Graph::const_iterator
Graph::begin() const
{
    return _vertices.begin();
}


inline
Graph::const_iterator
Graph::end() const
{
    return _vertices.end();
}


inline
Graph::reverse_iterator
Graph::rbegin()
{
    return _vertices.rbegin();
}


inline
Graph::reverse_iterator
Graph::rend()
{
    return _vertices.rend();
}

inline
Graph::const_reverse_iterator
Graph::rbegin() const
{
    return _vertices.rbegin();
}


inline
Graph::const_reverse_iterator
Graph::rend() const
{
    return _vertices.rend();
}


namespace carto
{
  DECLARE_GENERIC_OBJECT_TYPE( Graph * )
}

#endif
