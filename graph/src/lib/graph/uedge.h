#ifndef GRAPH_GRAPH_UEDGE_H
#define GRAPH_GRAPH_UEDGE_H


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

/**	Undirected edges.
	See Graph for UEdge creation.
*/
class GRAPH_API UEdge : public Edge
{
	/*	Graph factories call the constructor
	*/
	friend class GraphFactory;

public:

	virtual ~UEdge();

	//---------------------------------------------------------------------
	/**	@name Property querying methods*/
	//---------------------------------------------------------------------
	//@{

	/**	Is this edge directed?
		@return false
	*/
	virtual bool isDirected() const;

	/**	Is this edge undirected?
		@return true
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
	UEdge(std::string s);

	/**	The programmer should not call the copy constructor
		@param x edge to clone
		@see Graph for creating edges
	*/
	UEdge(const UEdge& x);

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
	UEdge& operator=(const UEdge&);

	//@}
};


#endif
