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

#include <graph/graph/gwriter_python.h>
#include <graph/graph/graph.h>
#include <graph/tree/tree.h>
#include <cartobase/wip/io/asciidatasourcetraits.h>
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
  const Graph	& g = dynamic_cast<const Graph &>( obj );
  if( !&g )
    cerr << "Warning: Graph helper used on something which is not a Graph\n";
  else if( g.order() > 0 )
    {
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
      if( g.size() > 0 )
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
  const Tree	& t = dynamic_cast<const Tree &>( obj );
  if( !&t )
    cerr << "Warning: Tree helper used on something which is not a Tree\n";
  else if( t.size() > 0 )
    {
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


