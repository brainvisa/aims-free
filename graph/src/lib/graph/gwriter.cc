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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/gwriter.h>
#include <graph/graph/graph.h>
#include <graph/graph/uedge.h>
#include <graph/graph/dedge.h>
#include <cartobase/object/lexicon.h>
#include <typeinfo>
#include <assert.h>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphWriter::GraphWriter(const string& filename,
			 const SyntaxSet& syntax,
			 const HelperSet& helpers)
	: AttributedWriter(filename, syntax, helpers), _index(0)
{
}


GraphWriter::GraphWriter(const SyntaxSet& syntax,
			 const HelperSet& helpers)
	: AttributedWriter(syntax, helpers), _index(0)
{
}


GraphWriter::~GraphWriter()
{
}


void
GraphWriter::write(const Graph& graph)
{
  (*_stream) << "# graph 1.0\n" << endl;

  (*_stream) << Lexicon::begin() << ' '
	     << Lexicon::graph() << ' '
	     << graph.getInterface<SyntaxedInterface>()->getSyntax() << endl;

  writeAttributes(&graph);

  const set<Vertex*> vertex = graph.vertices();	
  for (set<Vertex*>::const_iterator v = vertex.begin(); v != vertex.end(); ++v)
    {
      (*_stream) << '\n';
      write(**v);
    }

  const set<Edge*> edges = graph.edges();	
  for (set<Edge*>::const_iterator e = edges.begin(); e != edges.end(); ++e)
    {
      (*_stream) << '\n';
      write(**e);
    }

  (*_stream) << '\n'<< Lexicon::end() << endl;
}


void
GraphWriter::write(const Vertex& vertex)
{
  assert(_serializer.find(&vertex) == _serializer.end());
  _serializer[&vertex] = ++_index;

  (*_stream) << Lexicon::begin() << ' '
	     << Lexicon::vertex() << ' '
	     << vertex.getInterface<SyntaxedInterface>()->getSyntax() << ' '
	     << _index << endl;

  writeAttributes(&vertex);

  (*_stream) << Lexicon::end() << endl;
}


void
GraphWriter::write(const Edge& edge)
{
  (*_stream) << Lexicon::begin() << ' ';
  if ( dynamic_cast<const UEdge *>( &edge ) )
    {
      (*_stream) << Lexicon::uedge();
    }
  else if ( dynamic_cast<const DEdge *>( &edge ) )
    {
      (*_stream) << Lexicon::dedge();
    }
  else
    {
      // Hmmm?
      cerr << "unknown edge type" << endl;
      // write something anyway otherwise we lose the data
      (*_stream) << Lexicon::uedge();
    }
  (*_stream) << ' ' << edge.getInterface<SyntaxedInterface>()->getSyntax();

  for (Edge::const_iterator v = edge.begin(); v != edge.end(); ++v)
    {
      map<const Vertex*, int>::iterator v2 = _serializer.find(*v);
      assert(v2 != _serializer.end());
      (*_stream) << ' ' << v2->second;
    }

  (*_stream) << endl;
	
  writeAttributes(&edge);

  (*_stream) << Lexicon::end() << endl;
}


GraphWriter&
operator<<(GraphWriter& writer, const Graph& graph)
{
  writer.write(graph);
  return writer;
}
