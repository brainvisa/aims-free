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

#include <aims/io/selectionw.h>
#include <aims/selection/selection.h>
#include <graph/tree/tree.h>
#include <cartobase/object/syntax.h>
#if defined( _WIN32 ) && !defined( AIMS_SELECTION_TREE_OUTPUT )
// on windows, save in tree/shfj format
#define AIMS_SELECTION_TREE_OUTPUT
#endif
#ifdef AIMS_SELECTION_TREE_OUTPUT
#include <graph/tree/twriter.h>
#else
#include <graph/graph/gwriter_python.h>
#endif
#include <memory>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;

SelectionWriter::SelectionWriter( const string & filename )
  : _filename( filename ), _stream( 0 )
{
}


SelectionWriter::SelectionWriter( ostream & ostr )
  : _stream( &ostr )
{
}


SelectionWriter::~SelectionWriter()
{
}


void SelectionWriter::open( const string & filename )
{
  _stream = 0;
  _filename = filename;
}


void SelectionWriter::open( ostream & ostr )
{
  _stream = &ostr;
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


void SelectionWriter::write( const SelectionSet & select )
{
  auto_ptr<ostream>	s2;
  ostream	*stream;
  if( _stream )
    stream = _stream;
  else
    {
      s2.reset( new ofstream( _filename.c_str() ) );
      stream = s2.get();
    }

  // 1. convert to Tree

  Tree				t( true, "selector" );
  Tree				*t2, *t3;
  SelectionSet::const_iterator	i, e = select.end();
  string			name;
  Selection::const_iterator	is, es;

  for( i=select.begin(); i!=e; ++i )
    {
      t2 = new Tree( true, "subset" );
      name = i->name();
      if( !name.empty() )
        t2->setProperty( "surname", name );
      t.insert( t2 );
      for( is=i->begin(), es=i->end(); is!=es; ++is )
        {
          t3 = new Tree( true, "subset" );
          t3->setProperty( "nomenName", *is );
          t2->insert( t3 );
        }
    }

  // 2. save (Graph/Tree or python format)

#ifdef AIMS_SELECTION_TREE_OUTPUT
  const SyntaxSet	& ss = selectionSyntax();

  TreeWriter	tw( ss );
  tw.attach( *stream );
  tw << t;

#else

  GraphWriter_Python	pw;
  pw.attach( *stream );
  pw.write( t );
#endif
}


