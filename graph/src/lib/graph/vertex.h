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

#ifndef GRAPH_GRAPH_VERTEX_H
#define GRAPH_GRAPH_VERTEX_H


//=============================================================================
//    H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#ifndef GRAPH_GRAPH_GRAPHOBJECT_H
#include <graph/graph/graphobject.h>
#endif
#include <set>


//=============================================================================
//    F O R W A R D  D E C L A R A T I O N S
//=============================================================================

class Edge;


//=============================================================================
//    C L A S S  D E C L A R A T I O N
//=============================================================================

/**    Vertices are created and managed by \link Graph Graphs\endlink.
    See Graph for vertex creation.
*/
class GRAPH_API Vertex : public GraphObject
{

private:
    typedef std::set<Edge*> ESet;


public:
    typedef ESet::value_type             value_type;
#ifndef _WIN32
    ///    pointer is absent from MS Visual C++ / Intel Win32
    typedef ESet::pointer                pointer;
#endif
    typedef ESet::reference              reference;
    typedef ESet::const_reference        const_reference;
    typedef ESet::iterator               iterator;
    typedef ESet::const_iterator         const_iterator;
    typedef ESet::reverse_iterator       reverse_iterator;
    typedef ESet::const_reverse_iterator const_reverse_iterator;

    //---------------------------------------------------------------------
    /**    @name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    virtual ~Vertex();

    //@}

    //---------------------------------------------------------------------
    /**    @name Edge handling*/
    //---------------------------------------------------------------------
    //@{

    /** \deprecated{specialization deprecated because ambiguous: use
        edgeSize() for the number of edges. In a future release, size()
        will return the number properties as it does in GenericObject} */
    size_t size() const
#ifndef AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
      __attribute__((__deprecated__("use edgesSize() for "
        "the number of edges. In a future release, size() will return the "
        "number properties as it does in GenericObject")))
#endif
        ;
    /**    The edgesSize of a vertex is the number of attached edges
        @return the number of edges attached to the vertex
    */
    size_t edgesSize() const;

    /**    Is a given edge attached to this vertex?
        @param edge tested edge
        @return true if the edge is attached to this vertex
    */
    bool hasEdge(const Edge* edge) const;

    /**     Returns the edges linking this vertex to a given vertex
        @param vertex is the sink vertex
        @return the set of vertices for which this vertex is the
        source and vertex is the sink
    */
    std::set<Edge*> edgesTo(const Vertex* vertex) const;

    //@}

    //---------------------------------------------------------------------
    /**    @name Iterators iterate on the edges*/
    //---------------------------------------------------------------------
    //@{

    /**    Get the beginning of the edge collection
        @return an iterator pointing to the beginning of the
                edge collection
    */
    iterator               begin();

    /**    Get the end of the edge collection
        @return an iterator pointing to the end of the
                edge collection
    */
    iterator           end();

    /**    Get the beginning of the edge collection
        @return a const_iterator pointing to the beginning of the
                edge collection
    */
    const_iterator         begin() const;

    /**    Get the end of the edge collection
        @return a const_iterator pointing to the end of the
                edge collection
    */
    const_iterator         end() const;

    /**    Get the beginning of the reversed edge collection
        @return a reverse_iterator pointing to the beginning of the
                reversed edge collection
    */
    reverse_iterator       rbegin();

    /**    Get the end of the reversed edge collection
        @return a reverse_iterator pointing to the end of the
                reversed edge collection
    */
    reverse_iterator       rend();

    /**    Get the beginning of the reversed edge collection
        @return a const_reverse_iterator pointing to the end of the
                reversed edge collection
    */
    const_reverse_iterator rbegin() const;

    /**    Get the end of the reversed edge collection
        @return a const_reverse_iterator pointing to the end of the
                reversed edge collection
    */
    const_reverse_iterator rend() const;

    //@}

    //---------------------------------------------------------------------
    /**    @name Neighbours*/
    //---------------------------------------------------------------------
    //@{

    /**    Same as outNeighbours.
        Still don't know why it works with directed edges!
    */
    std::set<Vertex*> neighbours() const;

    /**    Find the vertices for which this vertex is the sink
        @return the set of vertices for which this vertex is the sink
    */
    std::set<Vertex*> inNeighbours() const;

    /**    Find the vertices for which this vertex is the source
        @return the set of vertices for which this vertex is the source
    */
    std::set<Vertex*> outNeighbours() const;

    /**    Return a random neighbour (CAUTION! not perfectly random)
        @return a random neighbour
    */
    Vertex* randomNeighbour() const;

    //@}

protected:

    //---------------------------------------------------------------------
    /**    @name Constructors, Destructor*/
    //---------------------------------------------------------------------
    //@{

    /**    The programmer should not call the constructor.
        @param s syntactic attribute of the new vertex
        @see Graph for creating vertices
    */
    Vertex(std::string s);

    /**    The programmer should not call the copy constructor.
        @param x vertex to clone
        @see Graph for creating vertices
    */
    Vertex(const Vertex& x);

    /**    The programmer should not clone
        @return a copy of this vertex
        @see Graph for creating vertices
    */
        virtual Vertex * cloneVertex() const;

    //@}

private:

    //---------------------------------------------------------------------
    /**    @name Friends*/
    //---------------------------------------------------------------------
    //@{

    ///    export addEdge and removeEdge
    friend class Graph;
    ///    export the constructors
    friend class GraphFactory;

    //@}

    //---------------------------------------------------------------------
    /**    @name Edge handling*/
    //---------------------------------------------------------------------
    //@{

    /**    For friends only!
        Add an edge to the edge collection.
        @param edge edge to add to the collection
    */
    void addEdge(Edge* edge);

    /**    For friends only!
        Remove an edge from the edge collection.
        @param edge edge to remove from the collection
    */
    void removeEdge(Edge* edge);

    //@}

    //---------------------------------------------------------------------
    /**    @name Disable copy*/
    //---------------------------------------------------------------------
    //@{

    /**    Assignment operator
    */
    Vertex& operator=(const Vertex&);

    //@}

    /**    Collection of edges the vertex is attached to
    */
    ESet _edges;

};


//=============================================================================
//    I N L I N E  M E T H O D S
//=============================================================================

inline
Vertex::iterator
Vertex::begin()
{
    return _edges.begin();
}


inline
Vertex::iterator
Vertex::end()
{
    return _edges.end();
}


inline
Vertex::const_iterator
Vertex::begin() const
{
    return _edges.begin();
}


inline
Vertex::const_iterator
Vertex::end() const
{
    return _edges.end();
}


inline
Vertex::reverse_iterator
Vertex::rbegin()
{
    return _edges.rbegin();
}


inline
Vertex::reverse_iterator
Vertex::rend()
{
    return _edges.rend();
}


inline
Vertex::const_reverse_iterator
Vertex::rbegin() const
{
    return _edges.rbegin();
}


inline
Vertex::const_reverse_iterator
Vertex::rend() const
{
    return _edges.rend();
}


inline
std::set<Vertex*>
Vertex::neighbours() const
{
    return outNeighbours();
}


namespace carto
{
  DECLARE_GENERIC_OBJECT_TYPE( Vertex * )
}

#endif
