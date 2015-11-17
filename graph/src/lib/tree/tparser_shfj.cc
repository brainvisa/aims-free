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

//--- header files ------------------------------------------------------------

#include <graph/tree/tparser_shfj.h>
#include <cartobase/object/lexicon.h>
#include <graph/tree/tree.h>
#include <graph/tree/tfactory.h>
#include <cartobase/exception/parse.h>
#include <soma-io/datasource/filteringdatasource.h>
#include <soma-io/utilities/asciidatasourcetraits.h>

using namespace carto;
using namespace std;


//--- methods -----------------------------------------------------------------

TreeParser_shfj::TreeParser_shfj( const SyntaxSet& syntax, 
				  const HelperSet& helpers )
  : AttributedReader( syntax, helpers ), _factory( new TreeFactory ), 
    _topTree( 0 )
{
}


TreeParser_shfj::TreeParser_shfj( const TreeFactory & factory, 
				  const string& filename,
				  const SyntaxSet& syntax, 
				  const HelperSet& helpers )
  : AttributedReader( filename, syntax, helpers ), TreeVisitor(), 
    _factory( factory.clone() ), _topTree( 0 )
{
}


TreeParser_shfj::TreeParser_shfj( const TreeFactory & factory, 
				  const SyntaxSet& syntax, 
				  const HelperSet& helpers )
  : AttributedReader( syntax, helpers ), TreeVisitor(), 
    _factory( factory.clone() ), _topTree( 0 )
{
}


TreeParser_shfj::~TreeParser_shfj()
{
  delete _factory;
}


void TreeParser_shfj::visitTree( Tree* tree )
{
  readTree( tree );
}


void TreeParser_shfj::readTree( Tree* tree )
{
  // cout << "TreeParser_shfj::readTree\n";
  string token;

  FilteringDataSource	ds( dataSource() );

  StreamUtil::skip( ds );
  string::size_type n = Lexicon::begin().length(), i;
  char c;
  for( i=0; i<n; ++i )
  {
    c = ds.getch();
    if( c != Lexicon::begin()[i] )
    {
      ds.ungetch( c );
      if( i > 0 )
        for( ; i>0; --i )
          ds.ungetch( Lexicon::begin()[i-1] );
      throw parse_error(Lexicon::begin(), token, name(), line());
    }
  }

  StreamUtil::skip( ds );
  token = StreamUtil::readUntil( ds );
  if (token != Lexicon::tree())
    throw parse_error(Lexicon::tree(), token, \
		      name(), line());
  tree->clear();
  tree->clearProperties();

  if( !_topTree )
    {	// read version only on the top-level tree
      StreamUtil::skip( ds );
      token = StreamUtil::readUntil( ds );
      if (token == "1.0")
        {
          StreamUtil::skip( ds );
          token = StreamUtil::readUntil( ds );
        }
      _topTree = tree;
    }
  else
    {
      StreamUtil::skip( ds );
      token = StreamUtil::readUntil( ds );
    }

  if (_rules.find(token) == _rules.end())
    throw parse_error("{syntactic attribute}", token, \
		      name(), line());
  // cout << "Tree syntax: " << token << endl;
  tree->getInterface<SyntaxedInterface>()->setSyntax(token);

  StreamUtil::skip( ds );
  token = StreamUtil::readUntil( ds );
  while (/*!token.empty() &&*/ token != Lexicon::end())
    {
      if (eof())
	throw parse_error(Lexicon::end(), token, \
			  name(), line());
      if (token == Lexicon::begin())
	{
          StreamUtil::skip( ds );
	  token = StreamUtil::readUntil( ds );
	  if (token == Lexicon::tree())
	    tree->insert( readSubTree() );
	  else
	    throw parse_error("{graph component}", token, \
			      name(), line());
	}
      else
        readAttribute(*tree, token);
      StreamUtil::skip( ds );
      token = StreamUtil::readUntil( ds );
    }

  if( _topTree == tree )
    _topTree = 0;	// release top-level tree
  else
    {
      set<string> missing;
      if( !tree->check( _rules, missing ) )
	{
	  string s( "missing semantic attribute " );
	  s+= *( missing.begin() );

	  set<string>::const_iterator	i, e = missing.end();
	  for( i=missing.begin(), ++i; i!=e; ++i )
	    s += string( ", " ) + *i;
	  throw range_error( s.c_str() );
	}
    }
}


Tree* TreeParser_shfj::readSubTree()
{
  string token;
  DataSource	& ds = *dataSource();

  if( !_topTree )
    {	// read version only on the top-level tree
      StreamUtil::skip( ds );
      token = StreamUtil::readUntil( ds );
    }

  if( _topTree || token == Lexicon::version() )
    {
      StreamUtil::skip( ds );
      token = StreamUtil::readUntil( ds );
    }
  if (_rules.find(token) == _rules.end())
    throw parse_error("{valid syntactic attribute}", token,
		      name(), line());

  string syntactic = token;

  Tree* tree = _factory->makeTree( syntactic );
  //cout << "Tree syntax 2: " << tree->getSyntax() << endl;
  if( !_topTree )
    _topTree = tree;

  StreamUtil::skip( ds );
  token = StreamUtil::readUntil( ds );
  while (/*!token.empty() &&*/ token != Lexicon::end())
    {
      if (eof())
	throw parse_error(Lexicon::end(), token,
			  name(), line());

      if (token == Lexicon::begin())
	{
          StreamUtil::skip( ds );
	  token = StreamUtil::readUntil( ds );
	  if (token == Lexicon::tree())
	    tree->insert( readSubTree() );
	  else
	    throw parse_error("{tree component}", token, \
			      name(), line());
	}
      else
        readAttribute(*tree, token);
      StreamUtil::skip( ds );
      token = StreamUtil::readUntil( ds );
    }

  if( _topTree == tree )
    _topTree = 0;	// release top-level tree

  set<string> missing;
  if( !tree->check( _rules, missing ) )
    {
      string s("missing semantic attribute(s): ");
      s+= *( missing.begin() );

      set<string>::const_iterator	i, e = missing.end();
      for( i=missing.begin(), ++i; i!=e; ++i )
	s += string( ", " ) + *i;
      throw range_error( s.c_str() );
    }

  return( tree );
}


Tree* TreeParser_shfj::read()
{
  string token;

  FilteringDataSource	ds( dataSource() );

  if( ds.eof() )
    throw parse_error(Lexicon::begin(), "<EOF>", name(), line());

  StreamUtil::skip( ds );
  token = StreamUtil::readUntil( ds );
  if (token != Lexicon::begin())
    throw parse_error(Lexicon::begin(), token, name(), line());

  StreamUtil::skip( ds );
  token = StreamUtil::readUntil( ds );
  if (token != Lexicon::tree())
    throw parse_error(Lexicon::tree(), token, name(), line());

   return( readSubTree() );
}


void TreeParser_shfj::read( Tree & tree )
{
  readTree( &tree );
}


void TreeParser_shfj::close()
{
  AttributedReader::close();
}


void TreeParser_shfj::attach( rc_ptr<DataSource> ds )
{
  AttributedReader::attach( ds );
}


void TreeParser_shfj::detach()
{
  AttributedReader::attach( DataSource::none() );
}


rc_ptr<DataSource> TreeParser_shfj::dataSource()
{
  return AttributedReader::dataSource();
}


string TreeParser_shfj::name() const
{
  return AttributedReader::name();
}


