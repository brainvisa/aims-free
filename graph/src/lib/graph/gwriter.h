#ifndef GRAPH_GRAPH_GWRITER_H
#define GRAPH_GRAPH_GWRITER_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#include <soma-io/writer/awriter.h>


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
