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

#include <graph/graph/gparser_python.h>
#include <graph/graph/graph.h>
#include <graph/tree/tree.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/datasource.h>
#include <iostream>

using namespace carto;
using namespace std;


static GenericObject* edgesHelper( GenericObject* parent, const string &, 
                                   PythonReader & r );

static GenericObject* vertexHelper( GenericObject* parent, const string & id, 
                                    PythonReader & r )
{
  //cout << "vertexHelper\n";
  Graph			*g = dynamic_cast<Graph *>( parent );
  GraphParser_Python	*gr = dynamic_cast<GraphParser_Python *>( &r );
  if( !g || !gr )
    {
      cerr << "vertexHelper is not used on a Graph !\n";
      // default to standard helper
      return r.dictHelper( parent, id, r );
    }

  Vertex	*v = g->addVertex( "" );
  gr->_vertextable[ id ] = v;
  r.readDictionary( *v );
  //cout << "vertex added, syntax: " << v->getSyntax() << endl;
  return 0;
}


static GenericObject* verticesHelper( GenericObject* parent, const string &, 
                                      PythonReader & r )
{
  //cout << "verticesHelper\n";
  Graph			*g = dynamic_cast<Graph *>( parent );
  // replace dictionary helper by vertex helper
  PythonReader::HelperSet	&hs = r.helpers();
  PythonReader::Helper		dic = hs[ "dictionary" ];
  hs[ "dictionary" ] = &vertexHelper;

  try
    {
      r.readDictionary( *g );
    }
  catch( exception & e )
    {
      // back to standard dict helper
      hs[ "dictionary" ] = dic;
      throw;
    }

  // back to standard dict helper
  hs[ "dictionary" ] = dic;

  if( !g->getInterface<SyntaxedInterface>()->getSyntax().empty() )
    {
      GraphParser_Python	*gr = dynamic_cast<GraphParser_Python *>( &r );
      if( gr )
	gr->syntaxes()[ g->getInterface<SyntaxedInterface>()->getSyntax() ]
          [ "__edges__" ].type = "edges";
    }

  //cout << "vertices: " << g->order() << endl;
  return( 0 );
}


static GenericObject* edgesHelper( GenericObject* parent, const string &, 
                                   PythonReader & r )
{
  //cout << "edgesHelper\n";
  Graph			*g = dynamic_cast<Graph *>( parent );
  GraphParser_Python	*gr = dynamic_cast<GraphParser_Python *>( &r );
  if( !g || !gr )
    {
      cerr << "edgesHelper is not used on a Graph !\n";
      // default to standard helper
      return 0;
    }

  bool			end = false;
  string		id, type;
  DataSource		& ds = *r.dataSource();
  SyntaxedInterface	*si = g->getInterface<SyntaxedInterface>();

  if( !ds.isOpen() )
    return( 0 );
  r.skipWhile( " \t\n\\" );
  char	c = ds.getch(), mark;
  if( c == '(' )
    mark = ')';
  else if( c == '[' )
    mark = ']';
  else
    {
      //cout << "char: " << c << endl;
      throw invalid_format_error( string( "PythonReader: Not a list/tuple, "
					  "line " ) + r.lineString() );
    }

  do
    {
      r.skipWhile( " \t\n\\" );
      c = ds.getch();
      if( c == ',' )	// separator
	{
	  r.skipWhile( " \t\n\\" );
	  c = ds.getch();
	}
      if( c == mark )
	{
	  end = true;
	  break;	// end
	}
      ds.ungetch( c );

      if( !si->getSyntax().empty() )
	gr->syntaxes()[ si->getSyntax() ][ "__edge__" ].type = "edge";
      r.read( parent, "__edge__" );
    }
  while( ds.isOpen() );

  if( !si->getSyntax().empty() )
    gr->syntaxes()[ si->getSyntax() ][ "__vertices__" ].type = "vertices";

  return 0;
}


static GenericObject* edgeHelper( GenericObject* parent, const string &, 
                                  PythonReader & r )
{
  //cout << "edgeHelper\n";
  Graph			*g = dynamic_cast<Graph *>( parent );
  GraphParser_Python	*gr = dynamic_cast<GraphParser_Python *>( &r );
  if( !g || !gr )
    {
      cerr << "edgeHelper is not used on a Graph !\n";
      // default to standard helper
      return 0;
    }

  // replace dictionary helper by vertex helper
  PythonReader::HelperSet	&hs = r.helpers();
  PythonReader::Helper		dic = hs[ "vertices" ];
  hs.erase( "vertices" );

  SyntaxedObject<Dictionary>	e( "" );
  r.readDictionary( e );
  Object	edges = e.getValue()[ "__vertices__" ];
  TypedObject<ObjectVector> 
    *ele = dynamic_cast<TypedObject<ObjectVector> *>( edges.get() );
  string				id;
  set<Vertex *>				vert;
  const map<string,Vertex *>		& vtable = gr->_vertextable;
  map<string,Vertex *>::const_iterator	it, et = vtable.end();

  if( ele )
    {
      vector<Object>::iterator	il, el = ele->getValue().end();
      for( il=ele->getValue().begin(); il!=el; ++il )
	{
	  id = (*il)->getString();
	  it = vtable.find( id );
	  if( it == et )
	    cerr << "edge to unknown vertex " << id << endl;
	  else
	    vert.insert( it->second );
	}
      if( vert.size() == 2 )
	{
	  Edge	*edge = g->addEdge( *vert.begin(), *vert.rbegin(), 
				    e.getInterface<SyntaxedInterface>()->
                                    getSyntax() );
	  e.removeProperty( "__vertices__" );
	  edge->copyProperties( Object::reference( e.value<Dictionary>() ) );
	}
      else
	cerr << "Not a binary edge: link " << vert.size() << " vertices\n";
    }

  hs[ "vertices" ] = dic;
  return 0;
}


static GenericObject* treesHelper( GenericObject* parent, const string &, 
                                   PythonReader & r )
{
  Tree			*g = dynamic_cast<Tree *>( parent );
  GraphParser_Python	*gr = dynamic_cast<GraphParser_Python *>( &r );
  if( !g || !gr )
    {
      cerr << "treesHelper is not used on a Tree !\n";
      // default to standard helper
      return 0;
    }

  bool		end = false;
  string	id, type;
  DataSource	& ds = *r.dataSource();

  if( !ds.isOpen() )
    return( 0 );
  r.skipWhile( " \t\n\\" );
  char	c = ds.getch(), mark;
  if( c == '(' )
    mark = ')';
  else if( c == '[' )
    mark = ']';
  else
    {
      //cout << "char: " << c << endl;
      throw invalid_format_error( string( "PythonReader: Not a list/tuple, "
					  "line " ) + r.lineString() );
    }

  do
    {
      r.skipWhile( " \t\n\\" );
      c = ds.getch();
      if( c == ',' )	// separator
	{
	  r.skipWhile( " \t\n\\" );
	  c = ds.getch();
	}
      if( c == mark )
	{
	  end = true;
	  break;	// end
	}
      ds.ungetch( c );

      if( !g->getInterface<SyntaxedInterface>()->getSyntax().empty() )
	gr->syntaxes()[ g->getInterface<SyntaxedInterface>()->getSyntax() ]
          [ "__tree__" ].type = "tree";
      r.read( parent, "__tree__" );
    }
  while( ds.isOpen() );

  return 0;
}


static GenericObject* treeHelper( GenericObject* parent, const string & id, 
                                  PythonReader & r )
{
  Tree			*g = dynamic_cast<Tree *>( parent );
  GraphParser_Python	*gr = dynamic_cast<GraphParser_Python *>( &r );
  if( !g || !gr )
    {
      cerr << "treeHelper is not used on a Tree !\n";
      // default to standard helper
      return r.dictHelper( parent, id, r );
    }

  Tree	*t = new Tree( true, "" );
  if( g )
    g->insert( t );

  try
    {
      r.readDictionary( *t );
    }
  catch( exception & )
    {
      if( g )
	g->remove( t );
      delete t;
      t = 0;
    }

  if( t && !t->getInterface<SyntaxedInterface>()->getSyntax().empty() )
    gr->syntaxes()[ t->getInterface<SyntaxedInterface>()->getSyntax() ]
      [ "__trees__" ].type = "trees";

  if( g )
    return 0;
  return t;
}


GraphParser_Python::GraphParser_Python( const SyntaxSet & syntax,
					const HelperSet & helpers )
  : GraphParser(), PythonReader( defaultSyntax(), helpers )
{
  addSyntax( syntax );
}


GraphParser_Python::~GraphParser_Python()
{
}


const GraphParser_Python::HelperSet & GraphParser_Python::defaultHelperSet()
{
  static HelperSet	hs;

  if( hs.empty() )
    {
      hs[ "vertices" ] = &verticesHelper;
      hs[ "vertex"   ] = &vertexHelper;
      hs[ "edges"    ] = &edgesHelper;
      hs[ "edge"     ] = &edgeHelper;
      hs[ "trees"    ] = &treesHelper;
      hs[ "tree"     ] = &treeHelper;
    }

  return hs;
}


const SyntaxSet & GraphParser_Python::defaultSyntax()
{
  static SyntaxSet	ss;

  if( ss.empty() )
    {
      Syntax	& s = ss[ "__generic__" ];
      s[ "__vertices__" ].type = "vertices";
      s[ "__edges__"    ].type = "edges";
      s[ "__edge__"     ].type = "edge";
      s[ "__children__" ].type = "trees";
      s[ "__tree__"     ].type = "tree";
    }

  return ss;
}


void GraphParser_Python::addSyntax( const SyntaxSet & ss )
{
  SyntaxSet::const_iterator	iss, ess = ss.end();
  Syntax::const_iterator	is, es;

  for( iss=ss.begin(); iss!=ess; ++iss )
    {
      Syntax	& s = syntaxes()[ iss->first ];
      for( is=iss->second.begin(), es=iss->second.end(); is!=es; ++is )
	s[ is->first ] = is->second;
    }
}


void GraphParser_Python::attach( rc_ptr<DataSource> ds )
{
  GraphParser::attach( ds );
}


void GraphParser_Python::detach()
{
  GraphParser::detach();
  PythonReader::attach( DataSource::none() );
}


int GraphParser_Python::read( Graph& graph )
{
  PythonReader::attach( GraphParser::_datasource );

  graph.getInterface<SyntaxedInterface>()->setSyntax( "" );
  PythonReader::read( graph );
  return 0;
}

int GraphParser_Python::read( Tree & tree )
{
  PythonReader::attach( GraphParser::_datasource );

  tree.getInterface<SyntaxedInterface>()->setSyntax( "" );
  PythonReader::read( tree );
  return 0;
}


rc_ptr<DataSource> GraphParser_Python::dataSource()
{
  return GraphParser::dataSource();
}


