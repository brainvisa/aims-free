// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

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


#ifdef AIMS_SELECTION_TREE_OUTPUT
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
#endif


void SelectionWriter::write( const SelectionSet & select )
{
  unique_ptr<ostream>	s2;
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


