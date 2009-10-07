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

#include <graph/graph/gfactory.h>
#include <graph/graph/vertex.h>
#include <graph/graph/uedge.h>
#include <graph/graph/dedge.h>

using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphFactory::GraphFactory()
{
}


GraphFactory::~GraphFactory()
{
}


map<string, GraphFactory::Generator> & GraphFactory::_generators()
{
  static map<string, Generator> gen;
  return gen;
}


void GraphFactory::registerGenerator( const string & s, Generator g )
{
  _generators()[ s ] = g;
}


void GraphFactory::unregisterGenerator( const string & s )
{
  _generators().erase( s );
}


Vertex*
GraphFactory::makeVertex(const string& s)
{
  const map<string, Generator> & gen = _generators();
  map<string, Generator>::const_iterator	i = gen.find( s );
  if( i != gen.end() )
    return static_cast<Vertex *>( i->second( s ) );
  return new Vertex(s);
}


UEdge*
GraphFactory::makeUEdge(const string& s)
{
  const map<string, Generator> & gen = _generators();
  map<string, Generator>::const_iterator	i = gen.find( s );
  if( i != gen.end() )
    return static_cast<UEdge *>( i->second( s ) );
  return new UEdge(s);
}


DEdge*
GraphFactory::makeDEdge(const string& s)
{
  const map<string, Generator> & gen = _generators();
  map<string, Generator>::const_iterator	i = gen.find( s );
  if( i != gen.end() )
    return static_cast<DEdge *>( i->second( s ) );
  return new DEdge(s);
}
