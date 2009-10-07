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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/gprinter.h>
#include <graph/graph/graph.h>
#include <graph/graph/uedge.h>
#include <graph/graph/dedge.h>
#include <iostream>
#include <typeinfo>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphPrinter::GraphPrinter(ostream& os)
	: _os(os)
{
}


GraphPrinter::~GraphPrinter()
{
}


void
GraphPrinter::print(const Graph& graph)
{
	_os << "*BEGIN Graph " << ' '
            << graph.getInterface<SyntaxedInterface>()->getSyntax() << ' '
            << &graph << endl;

        Object	a;
        for( a=graph.objectIterator(); a->isValid(); a->next() )
          {
            _os << a->key() << endl;
          }

	const set<Vertex*> vertices = graph.vertices();
	for (set<Vertex*>::const_iterator v = vertices.begin();
		v != vertices.end(); ++v)
	{
		_os << endl;
		print(**v);
	}

	const set<Edge*> edges = graph.edges();
	for (set<Edge*>::const_iterator e = edges.begin();
		e != edges.end(); ++e)
	{
		_os << endl;
		print(**e);
	}

	_os << "\n*END" << endl;
}


void
GraphPrinter::print(const Vertex& vertex)
{
  _os << "*BEGIN Vertex " 
      << vertex.getInterface<SyntaxedInterface>()->getSyntax() << ' '
      << &vertex << " {";
	for (Vertex::const_iterator e = vertex.begin(); e != vertex.end(); ++e)
	{
		_os << ' ' << (*e);
	}
	_os << " }" << endl;

        Object	a;
        for( a=vertex.objectIterator(); a->isValid(); a->next() )
          {
            _os << a->key() << endl;
          }

	_os << "*END" << endl;
}


void
GraphPrinter::print(const Edge& edge)
{
	_os << "*BEGIN ";
	if (typeid(edge) == typeid(UEdge))
	{
		_os << "UEdge";
	}
	else if (typeid(edge) == typeid(DEdge))
	{
		_os << "DEdge";
 	}
	else
	{
		// Hmmm?
	}
	_os << ' ' << edge.getInterface<SyntaxedInterface>()->getSyntax() 
            << ' ' << &edge << " {";
	for (Edge::const_iterator v = edge.begin(); v != edge.end(); ++v)
	{
		_os << ' ' << (*v);
	}
	_os << " }" << endl;

        Object	a;
        for( a=edge.objectIterator(); a->isValid(); a->next() )
          {
            _os << a->key() << endl;
          }

	_os << "*END" << endl;
}


ostream&
operator<<(ostream& os, const Graph& graph)
{
	GraphPrinter printer(os);
	printer.print(graph);
	return os;
}
