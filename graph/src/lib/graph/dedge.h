#ifndef GRAPH_GRAPH_DEDGE_H
#define GRAPH_GRAPH_DEDGE_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#ifndef GRAPH_GRAPH_EDGE_H
#include <graph/graph/edge.h>
#endif


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	Directed edges.
	See Graph for DEdge creation.
*/
class GRAPH_API DEdge : public Edge
{
	/*	Graph factories call the constructor
	*/
	friend class GraphFactory;

public:

	virtual ~DEdge();

	//---------------------------------------------------------------------
	/**	@name Property querying methods*/
	//---------------------------------------------------------------------
	//@{

	/**	Is this edge directed?
		@return true
	*/
	virtual bool isDirected() const;

	/**	Is this edge undirected?
		@return false
	*/
	virtual bool isUndirected() const;

	//@}

protected:

	//---------------------------------------------------------------------
	/**	@name Constructors, destructor*/
	//---------------------------------------------------------------------
	//@{

	/**	The programmer should not call the constructor
		@param s syntactic attribute
	*/
	DEdge(std::string s);

	/**	The programmer should not call the copy constructor
		@param x edge to clone
		@see Graph for creating edges
	*/
	DEdge(const DEdge& x);

	/**	The programmer should not clone
		@return a copy of this edge
		@see Graph for creating edges
	*/
	virtual Edge* cloneEdge() const;

	//@}

private:

	//---------------------------------------------------------------------
	/**	@name Disable copy*/
	//---------------------------------------------------------------------
	//@{

	/**	Assignment operator
	*/
	DEdge& operator=(const DEdge&);

	//@}
};


#endif
