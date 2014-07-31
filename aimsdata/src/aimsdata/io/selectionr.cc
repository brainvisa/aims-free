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
#include <soma-io/datasource/streamdatasource.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/io/reader.h>
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
    static SyntaxSet    ss;
    if( ss.empty() )
      {
        ss[ "selector" ][ "graph_syntax" ].type = "string";
        Syntax  & s = ss[ "subset" ];
        s[ "surname" ].type = "string";
        s[ "nomenName" ].type = "string";
      }
    return ss;
  }


  void convertFromSel( Object gen, SelectionSet & select )
  {
    Object contents = gen->getProperty( "__children__" );
    Object iter, it2;
    Object element;
    string name;

    for( iter=contents->objectIterator(); iter->isValid(); iter->next() )
    {
      element = iter->currentValue();
      if( !element->getProperty( "surname", name ) )
        name = "";
      select.addSelection( Selection( name ) );
      Selection & sel = *select.rbegin();
      contents = element->getProperty( "__children__" );
      for( it2=contents->objectIterator(); it2->isValid(); it2->next() )
        if( it2->currentValue()->getProperty( "nomenName", name ) )
          sel.select( name );
    }
  }


  void convertFromJson( Object gen, SelectionSet & select )
  {
    // TODO: what do we do with the other, "brain",node ?
    Object contents = gen->getProperty( "customized group" );
    Object iter, it2;
    Object element;
    string name;

    for( iter=contents->objectIterator(); iter->isValid(); iter->next() )
    {
      name = iter->key();
      select.addSelection( Selection( name ) );
      Selection & sel = *select.rbegin();
      element = iter->currentValue();
      for( it2=element->objectIterator(); it2->isValid(); it2->next() )
        sel.select( it2->key() ); // TODO: value is not used and dropped
    }
  }


  void readAsTree( SelectionSet & select, istream *stream )
  {
    // 1. read as Tree

    const SyntaxSet       & ss = selectionSyntax();
    Tree                  t( true, "selector" );
    TreeReader            tr( ss );
    tr.attach( *stream );
    tr.read( t );
    tr.detach();

    // 2. convert to SelectionSet

    Tree::const_iterator  it, et = t.end(), it2, et2;
    string                name;
    Tree                  *t2;

    for( it=t.begin(); it!=et; ++it )
      {
        t2 = (Tree *) *it;
        if( !t2->getProperty( "surname", name ) )
          name = "";
        select.addSelection( Selection( name ) );
        Selection & sel = *select.rbegin();
        for( it2=t2->begin(), et2=t2->end(); it2!=et2; ++it2 )
          if( ((Tree *) *it2)->getProperty( "nomenName", name ) )
            sel.select( name );
      }
  }

}


void SelectionReader::read( SelectionSet & select )
{
  rc_ptr<DataSource> ds;
  if( _stream )
    ds.reset( new IStreamDataSource( *_stream ) );
  else
    ds.reset( new FileDataSource( _filename ) );

  // 1. read as dict Object

  carto::Reader<GenericObject> read( ds );
  try
  {
    Object gen( read.read() );

    // 2. convert to SelectionSet, 2 cases
    if( gen->size() == 1 && gen->hasProperty( "__children__" ) )
      // older .sel (.minf / Tree) case
      convertFromSel( gen, select );
    else // newer json (Atlas Viewer) case
      convertFromJson( gen, select );
  }
  catch( ... )
  {
    // maybe an old Tree format ?
    auto_ptr<istream>     s2;
    istream       *stream;
    if( _stream )
      stream = _stream;
    else
    {
      s2.reset( new ifstream( _filename.c_str() ) );
      stream = s2.get();
    }
    readAsTree( select, stream );
  }
}


