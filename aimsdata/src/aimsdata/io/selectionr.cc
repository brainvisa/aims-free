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

#include <aims/io/selectionr.h>
#include <aims/selection/selection.h>
#include <graph/tree/treader.h>
#include <graph/tree/tree.h>
#include <cartobase/object/syntax.h>
#include <memory>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;

SelectionReader::SelectionReader( const string & filename )
  : _filename( filename ), _stream( 0 )
{
}


SelectionReader::SelectionReader( istream & istr )
  : _stream( &istr )
{
}


SelectionReader::~SelectionReader()
{
}


void SelectionReader::open( const string & filename )
{
  _stream = 0;
  _filename = filename;
}


void SelectionReader::open( istream & istr )
{
  _stream = &istr;
  _filename = "";
}


namespace
{

  const SyntaxSet & selectionSyntax()
  {
    static SyntaxSet	ss;
    if( ss.empty() )
      {
        ss[ "selector" ][ "graph_syntax" ].type = "string";
        Syntax	& s = ss[ "subset" ];
        s[ "surname" ].type = "string";
        s[ "nomenName" ].type = "string";
      }
    return ss;
  }

}


void SelectionReader::read( SelectionSet & select )
{
  auto_ptr<istream>	s2;
  istream	*stream;
  if( _stream )
    stream = _stream;
  else
    {
      s2.reset( new ifstream( _filename.c_str() ) );
      stream = s2.get();
    }

  // 1. read as Tree

  const SyntaxSet	& ss = selectionSyntax();
  Tree			t( true, "selector" );
  TreeReader		tr( ss );
  tr.attach( *stream );
  tr.read( t );
  tr.detach();

  // 2. convert to SelectionSet

  Tree::const_iterator	it, et = t.end(), it2, et2;
  string		name;
  Tree			*t2;

  for( it=t.begin(); it!=et; ++it )
    {
      t2 = (Tree *) *it;
      if( !t2->getProperty( "surname", name ) )
        name = "";
      select.addSelection( Selection( name ) );
      Selection	& sel = *select.rbegin();
      for( it2=t2->begin(), et2=t2->end(); it2!=et2; ++it2 )
        if( ((Tree *) *it2)->getProperty( "nomenName", name ) )
          sel.select( name );
    }
}


