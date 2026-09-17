#ifndef GRAPH_GRAPH_GRAPHOBJECT_H
#define GRAPH_GRAPH_GRAPHOBJECT_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#include <cartobase/object/attributed.h>


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	The abstract base class for graphs, vertices and edges
*/
class GRAPH_API GraphObject : public carto::AttributedObject
{

public:

	//---------------------------------------------------------------------
	/**	@name Constructors, Destructor*/
	//---------------------------------------------------------------------
	//@{

	virtual ~GraphObject();

	//@}

protected:

	//---------------------------------------------------------------------
	/**	@name Constructors, Destructor*/
	//---------------------------------------------------------------------
	//@{

	/**	The programmer should not call the constructor of an
		abstract base class.
		@param s syntactic attribute of the new graph
	*/
	GraphObject(const std::string& s);

	GraphObject(const GraphObject& x);

	//@}

private:

	//---------------------------------------------------------------------
	/**	@name Disable copy*/
	//---------------------------------------------------------------------
	//@{

	GraphObject& operator=(const GraphObject&);

	//@}
};


#endif
