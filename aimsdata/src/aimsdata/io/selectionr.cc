// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

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
    unique_ptr<istream>     s2;
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


