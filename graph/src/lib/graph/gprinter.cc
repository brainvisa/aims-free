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
