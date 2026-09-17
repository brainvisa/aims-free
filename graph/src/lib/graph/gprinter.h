#ifndef GRAPH_GRAPH_GPRINTER_H
#define GRAPH_GRAPH_GPRINTER_H


//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

#include <graph/config/graph_config.h>
#include <iostream>

class Graph;
class Vertex;
class Edge;


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	Prints GraphObject
*/
class GRAPH_API GraphPrinter
{

public:

	GraphPrinter(std::ostream& os);

	virtual ~GraphPrinter();

	virtual void print(const Graph& graph);
	virtual void print(const Vertex& vertex);
	virtual void print(const Edge& edge);

private:

	//---------------------------------------------------------------------
	/**	@name Disable copy*/
	//---------------------------------------------------------------------
	//@{

	/// Copy constructor
	GraphPrinter(const GraphPrinter&);

	/// Assignment operator
	GraphPrinter& operator=(const GraphPrinter&);

	//@}

	//---------------------------------------------------------------------
	/**	@name Data*/
	//---------------------------------------------------------------------
	//@{

	/**	stream to print on
	*/
	std::ostream& _os;

	//@}

};


//=============================================================================
//	F U N C T I O N S
//=============================================================================

/**	Print a  Graph using a GraphPrinter
	@param os stream to print on
	@param graph graph to print
	@return the stream given as a parameter
*/
GRAPH_API std::ostream& operator<<(std::ostream& os, const Graph& graph);


#endif
