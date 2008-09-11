/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef GRAPH_GRAPH_GWRITER_H
#define GRAPH_GRAPH_GWRITER_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#include <cartobase/object/awriter.h>


//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

class Graph;
class Vertex;
class Edge;


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	To write a Graph to a file
*/
class GRAPH_API GraphWriter : public carto::AttributedWriter
{

public:

	//---------------------------------------------------------------------
	/**	@name Constructors, destructor*/
	//---------------------------------------------------------------------
	//@{

	GraphWriter(const std::string& filename,
		    const carto::SyntaxSet& syntax,
		    const HelperSet& helpers = HelperSet());

	GraphWriter(const carto::SyntaxSet& syntax,
		    const HelperSet& helpers = HelperSet());

	virtual ~GraphWriter();

	//@}

	//---------------------------------------------------------------------
	/**	@name Write method*/
	//---------------------------------------------------------------------
	//@{

	/**	write a Graph to a file
		@param graph graph to save
	*/
	virtual void write(const Graph& graph);

	//@}

protected:

	//---------------------------------------------------------------------
	/**	@name Write methods*/
	//---------------------------------------------------------------------
	//@{

	/**	write a Vertex to a file
		@param vertex vertex to save
	*/
	virtual void write(const Vertex& vertex);

	/**	write an Edge to a file
		@param edge undirected edge to save
	*/
	virtual void write(const Edge& edge);

	//@}

private:

	//---------------------------------------------------------------------
	/**	@name Disable copy*/
	//---------------------------------------------------------------------
	//@{

	/// Copy constructor
	GraphWriter(const GraphWriter&);

	/// Assignment operator
	GraphWriter& operator=(const GraphWriter&);

	//@}

	//---------------------------------------------------------------------
	/**	@name Data*/
	//---------------------------------------------------------------------
	//@{

	/// current vertex index in file
	int _index;

	/// vertex pointer serialization
	std::map<const Vertex*, int> _serializer;

	//@}
};


//=============================================================================
//	F U N C T I O N S
//=============================================================================

/**	Save a graph to a file using a GraphWriter
	@param writer GraphWriter to restore from
	@param graph graph to save
	@return the GraphWriter given as a parameter
*/
GRAPH_API GraphWriter& operator<<(GraphWriter& writer, const Graph& graph);


#endif 
