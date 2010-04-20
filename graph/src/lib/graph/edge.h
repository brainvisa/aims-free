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

#ifndef GRAPH_GRAPH_EDGE_H
#define GRAPH_GRAPH_EDGE_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#ifndef GRAPH_GRAPH_GRAPHOBJECT_H
#include <graph/graph/graphobject.h>
#endif
#include <cartobase/containers/block.h>


//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

class Vertex;


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	The abstract base class for all types of edges; edges are created and
	managed by \link Graph Graphs\endlink.
	Edge is the abstract base class for all types of edges, directed (D)
	or undirected (U). An Edge should contain at least one Vertex
	object. When the last vertex is removed from an edge, the edge is
	deleted by its graph owner. \n
	See Graph for Edge creation.
*/
class GRAPH_API Edge : public GraphObject
{

private:

	//---------------------------------------------------------------------
	/**	@name Associated types*/
	//---------------------------------------------------------------------
	//@{

	///
	typedef carto::block<Vertex*,2> VSet;

	//@}

public:

	//---------------------------------------------------------------------
	/**	@name Associated types*/
	//---------------------------------------------------------------------
	//@{

	///
	typedef VSet::value_type             value_type;
	///
	typedef VSet::pointer                pointer;
	///
	typedef VSet::reference              reference;
	///
	typedef VSet::const_reference        const_reference;
	///
	typedef VSet::iterator               iterator;
	///
	typedef VSet::const_iterator         const_iterator;
	///
	typedef VSet::reverse_iterator       reverse_iterator;
	///
	typedef VSet::const_reverse_iterator const_reverse_iterator;

	//@}

	//---------------------------------------------------------------------
	/**	@name Constructors, destructor*/
	//---------------------------------------------------------------------
	//@{

	/**	-
	*/
	virtual ~Edge();

	//@}

	//---------------------------------------------------------------------
	/**	@name Property querying methods*/
	//---------------------------------------------------------------------
	//@{

	/**	Is this edge directed?
	*/
	virtual bool isDirected() const = 0;

	/**	Is this edge undirected?
	*/
	virtual bool isUndirected() const = 0;

	//@}

	//---------------------------------------------------------------------
	/**	@name Vertex handling*/
	//---------------------------------------------------------------------
	//@{

	/**	Does this edge contain a given vertex?
		@param vertex pointer to the vertex to check
		@return true if the edge contains this vertex
	*/
	bool hasVertex(const Vertex* vertex) const;
	/// number of vertices
	size_t verticesSize() const;

	//@}

	//---------------------------------------------------------------------
	/**	@name Iterators iterate on the vertices*/
	//---------------------------------------------------------------------
	//@{

	/**	Get the beginning of the vertex collection of the graph
		@return an iterator pointing to the beginning of the
		        vertex collection
	*/
	iterator begin();

	/**	Get the end of the vertex collection of the graph
		@return an iterator pointing to the end of the
		        vertex collection
	*/
	iterator end();

	/**	Get the beginning of the vertex collection of the graph
		@return a const_iterator pointing to the beginning of the
		        vertex collection
	*/
	const_iterator begin() const;

	/**	Get the end of the vertex collection of the graph
		@return a const_iterator pointing to the end of the
		        vertex collection
	*/
	const_iterator end() const;

	/**	Get the beginning of the reversed vertex collection of
		the graph
		@return a reverse_iterator pointing to the beginning of the
		        reversed vertex collection
	*/
	reverse_iterator rbegin();

	/**	Get the end of the reversed vertex collection of
		the graph
		@return a reverse_iterator pointing to the end of the
		        reversed vertex collection
	*/
	reverse_iterator rend();

	/**	Get the beginning of the reversed vertex collection of
		the graph
		@return a const_reverse_iterator pointing to the beginning of
		        the reversed vertex collection
	*/
	const_reverse_iterator rbegin() const;

	/**	Get the end of the reversed vertex collection of
		the graph
		@return a const_reverse_iterator pointing to the end of the
		        reversed vertex collection
	*/
	const_reverse_iterator rend() const;

	//@}

protected:

	//---------------------------------------------------------------------
	/**	@name Constructors, destructor*/
	//---------------------------------------------------------------------
	//@{

	/**	The programmer should not call the constructor
		\param s syntactic attribute
		\sa Graph for creating edges
	*/
	Edge(std::string s);

	/**	The programmer should not call the copy constructor
		\param x edge to clone
		\sa Graph for creating edges
	*/
	Edge(const Edge& x);

	/**	The programmer should not clone
		\return a copy of this edge
		\sa Graph for creating edges
	*/
	virtual Edge* cloneEdge() const = 0;

	//@}

private:

	///	export private method addVertices
	friend class Graph;

	//---------------------------------------------------------------------
	/**	@name Vertex handling*/
	//---------------------------------------------------------------------
	//@{

	/**	For friends only!
		Add a pair of vertices to the vertex collection.
		@param vertex1 vertex to add
		@param vertex2 vertex to add
	*/
	void addVertices(Vertex* vertex1, Vertex* vertex2);

	/**	For friends only!
		Remove a vertex from the vertex collection.
		@param vertex vertex to remove
	*/
	void removeVertex(Vertex* vertex);

	//@}

	//---------------------------------------------------------------------
	/**	@name Disable copy*/
	//---------------------------------------------------------------------
	//@{

	/**	Assignment operator
	*/
	Edge& operator=(const Edge&);

	//@}

	//---------------------------------------------------------------------
	/**	@name Data*/
	//---------------------------------------------------------------------
	//@{

	/**	Collection of vertices in this edge. 
		Note that an edge is \b not responsible for the deletion
		of its vertices - the Graph is.
	*/
	VSet _vertices;

	//@}
};


//=============================================================================
//	I N L I N E  M E T H O D S
//=============================================================================

inline
bool
Edge::hasVertex(const Vertex* vertex) const
{
	return _vertices[0] == vertex || _vertices[1] == vertex;
}


inline
Edge::iterator
Edge::begin()
{
	return _vertices.begin();
}


inline
Edge::iterator
Edge::end()
{
	return _vertices.end();
}


inline
Edge::const_iterator
Edge::begin() const
{
	return _vertices.begin();
}


inline
Edge::const_iterator
Edge::end() const
{
	return _vertices.end();
}


inline
Edge::reverse_iterator
Edge::rbegin()
{
	return _vertices.rbegin();
}


inline
Edge::reverse_iterator
Edge::rend()
{
	return _vertices.rend();
}


inline
Edge::const_reverse_iterator
Edge::rbegin() const
{
	return _vertices.rbegin();
}


inline
Edge::const_reverse_iterator
Edge::rend() const
{
	return _vertices.rend();
}


namespace carto
{
  DECLARE_GENERIC_OBJECT_TYPE( Edge * )
}


#endif
