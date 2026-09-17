#include <graph/graph/gwriter_python.h>
#include <graph/graph/graph.h>
#include <graph/tree/tree.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <iostream>

using namespace carto;
using namespace std;

static void dictGraphHelper( const GenericObject & obj, PythonWriter & w, 
			     int indent, bool writeInternals )
{
  DataSource	& ds = *w.dataSource();
  Object	im;
  bool		first = true, hassyntax = false;
  string	ind, ind2;
  int		i;
  char		sep = '\n';

  if( w.singleLine() )
    sep = ' ';
  else
    {
      for( i=0; i<indent-1; ++i )
	ind += "    ";
      ind2 = ind + "  ";
      if( indent > 0 )
        ind += "    ";
      ++indent;
    }

  ds.putch( '{' );
  ds.putch( sep );
  AsciiDataSourceTraits<string>::write( ds, ind );

  string	synt;
  const SyntaxedInterface	*si = obj.getInterface<SyntaxedInterface>();
  if( si && si->hasSyntax() )
    {
      synt = si->getSyntax();
      hassyntax = true;
    }
  else
    {
      try
        {
          Object	sx = obj.getProperty( "__syntax__" );
          if( sx.get() )
            {
              synt = sx->GenericObject::value<string>();
              hassyntax = true;
            }
        }
      catch( ... )
        {
        }
    }

  if( hassyntax )
    {
      AsciiDataSourceTraits<string>::write( ds, "'__syntax__' : " );
      w.writeString( ds, synt );
      first = false;
    }

  for( im=obj.objectIterator(); im->isValid(); im->next() )
    if( ( writeInternals || !w.isInternal( "", im->key() ) ) 
	&& ( im->key() != "__syntax__" || !hassyntax ) )
      {
	if( first )
	  first = false;
	else
          {
            ds.putch( ',' );
            ds.putch( sep );
            AsciiDataSourceTraits<string>::write( ds, ind );
          }
	w.writeString( ds, im->key() );
        AsciiDataSourceTraits<string>::write( ds, " : " );
	w.write( *im->currentValue(), indent, "", im->key(), writeInternals );
      }
  // write vertices
  const Graph	* pg = dynamic_cast<const Graph *>( &obj );
  if( !pg )
    cerr << "Warning: Graph helper used on something which is not a Graph\n";
  else if( pg->order() > 0 )
    {
      const Graph& g = *pg;
      ds.putch( ',' );
      ds.putch( sep );
      AsciiDataSourceTraits<string>::write( ds, ind );
      AsciiDataSourceTraits<string>::write( ds, "'__vertices__' : [" );
      ds.putch( sep );
      AsciiDataSourceTraits<string>::write( ds, ind );
      AsciiDataSourceTraits<string>::write( ds, "  " );
      Graph::const_iterator	iv, ev = g.end();
      bool			first = true;
      for( iv=g.begin(); iv!=ev; ++iv )
        {
          if( first )
            first = false;
          else
            {
              ds.putch( ',' );
              ds.putch( sep );
              AsciiDataSourceTraits<string>::write( ds, ind );
              AsciiDataSourceTraits<string>::write( ds, "  " );
            }
          w.write( **iv, indent, "", "", writeInternals );
        }
      ds.putch( sep );
      AsciiDataSourceTraits<string>::write( ds, ind );
      AsciiDataSourceTraits<string>::write( ds, "  ]" );

      // write edges
      if( g.edgesSize() > 0 )
        {
          const set<Edge *>	& edg = g.edges();
          ds.putch( ',' );
          ds.putch( sep );
          AsciiDataSourceTraits<string>::write( ds, ind );
          AsciiDataSourceTraits<string>::write( ds, "'__edges__' : [" );
          ds.putch( sep );
          AsciiDataSourceTraits<string>::write( ds, ind );
          AsciiDataSourceTraits<string>::write( ds, "  " );
          set<Edge *>::const_iterator	ie, ee = edg.end();
          bool			first = true;
          for( ie=edg.begin(); ie!=ee; ++ie )
            {
              if( first )
                first = false;
              else
                {
                  ds.putch( ',' );
                  ds.putch( sep );
                  AsciiDataSourceTraits<string>::write( ds, ind );
                  AsciiDataSourceTraits<string>::write( ds, "  " );
                }
              w.write( **ie, indent, "", "", writeInternals );
            }
          ds.putch( ',' );
          ds.putch( sep );
          AsciiDataSourceTraits<string>::write( ds, ind );
          ds.putch( ']' );
        }
    }
  ds.putch( sep );
  AsciiDataSourceTraits<string>::write( ds, ind2 );
  ds.putch( '}' );
}


static void dictTreeHelper( const GenericObject & obj, PythonWriter & w, 
                            int indent, bool writeInternals )
{
  DataSource	& ds = *w.dataSource();
  Object	im;
  bool		first = true, hassyntax = false;
  string	ind, ind2;
  int		i;
  char		sep = '\n';

  if( w.singleLine() )
    sep = ' ';
  else
    {
      for( i=0; i<indent-1; ++i )
	ind += "    ";
      ind2 = ind + "  ";
      if( indent > 0 )
        ind += "    ";
      ++indent;
    }

  ds << string( "{" ) << sep << ind;

  string	synt;
  const SyntaxedInterface	*si = obj.getInterface<SyntaxedInterface>();
  if( si && si->hasSyntax() )
    {
      synt = si->getSyntax();
      hassyntax = true;
    }
  else
    {
      try
        {
          Object	sx = obj.getProperty( "__syntax__" );
          if( sx.get() )
            {
              synt = sx->GenericObject::value<string>();
              hassyntax = true;
            }
        }
      catch( ... )
        {
        }
    }

  if( hassyntax )
    {
      ds << "'__syntax__' : ";
      w.writeString( ds, synt );
      first = false;
    }

  for( im=obj.objectIterator(); im->isValid(); im->next() )
    if( ( writeInternals || !w.isInternal( "", im->key() ) ) 
	&& ( im->key() != "__syntax__" || !hassyntax ) )
      {
	if( first )
	  first = false;
	else
	  ds << "," << sep << ind;
	w.writeString( ds, im->key() );
	ds << " : ";
	w.write( *im->currentValue(), indent, "", im->key(), writeInternals );
      }
  // write subtrees
  const Tree	* pt = dynamic_cast<const Tree *>( &obj );
  if( !pt )
    cerr << "Warning: Tree helper used on something which is not a Tree\n";
  else if( pt->childrenSize() > 0 )
    {
      const Tree& t = *pt;
      ds << ',' << sep << ind << "'__children__' : [" << sep << ind << "  ";
      Tree::const_iterator	it, et = t.end();
      bool			first = true;
      for( it=t.begin(); it!=et; ++it )
        {
          if( first )
            first = false;
          else
            ds << "," << sep << ind << "  ";
          w.write( *(Tree*) *it, indent, "", "", writeInternals );
        }
      ds << sep << ind << "]";
    }
  ds << sep << ind2 << "}";
}


GraphWriter_Python::GraphWriter_Python( const std::string& filename,
					const SyntaxSet& syntax,
					const HelperSet& helpers )
  : PythonWriter( filename, syntax, helpers )
{
  const HelperSet	& hs = defaultHelpers();
  _helpers.insert( hs.begin(), hs.end() );
}


GraphWriter_Python::GraphWriter_Python( const SyntaxSet& syntax,
					const HelperSet& helpers )
  : PythonWriter( syntax, helpers )
{
  const HelperSet	& hs = defaultHelpers();
  _helpers.insert( hs.begin(), hs.end() );
}


GraphWriter_Python::~GraphWriter_Python()
{
}


const GraphWriter_Python::HelperSet & GraphWriter_Python::defaultHelpers()
{
  static HelperSet	h;

  if( h.empty() )
    {
      Graph	g;
      Tree	t;
      h[ g.type() ] = dictGraphHelper;
      h[ t.type() ] = dictTreeHelper;
    }

  return h;
}


