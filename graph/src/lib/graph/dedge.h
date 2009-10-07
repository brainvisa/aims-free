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
