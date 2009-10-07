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
