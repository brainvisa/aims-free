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

#include <graph/tree/treader.h>
#include <graph/tree/tparser_shfj.h>
#include <graph/tree/tparser_python.h>
#include <graph/tree/tfactory.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/datasource/streamdatasource.h>
#include <memory>
#include <fstream>

using namespace carto;
using namespace std;

//--- methods -----------------------------------------------------------------

TreeReader::TreeReader( const string& filename, const SyntaxSet& syntax, 
			const AttributedReader::HelperSet& helpers )
  : _syntax( syntax ), _helpers( helpers ), 
    _factory( 0 ), _datasource( new FileDataSource( filename ) )
{
}


TreeReader::TreeReader( const SyntaxSet& syntax, 
			const AttributedReader::HelperSet& helpers )
  : _syntax( syntax ), _helpers( helpers ), _factory( 0 )
{
}


TreeReader::TreeReader( const TreeFactory & factory, const string& filename,
			const SyntaxSet& syntax, 
			const AttributedReader::HelperSet& helpers )
  : _syntax( syntax ), _helpers( helpers ), 
    _factory( factory.clone() ), _datasource( new FileDataSource( filename ) )
{
}


TreeReader::TreeReader( const TreeFactory & factory, const SyntaxSet& syntax, 
			const AttributedReader::HelperSet& helpers )
  : _syntax( syntax ), _helpers( helpers ), _factory( factory.clone() )
{
}


TreeReader::~TreeReader()
{
  delete _factory;
}


void TreeReader::setSyntax( const SyntaxSet & syntax )
{
  _syntax = syntax;
}


TreeReader& operator >> ( TreeReader& reader, Tree& tree )
{
  reader.read( tree );
  return reader;
}


list<TreeParser *> & TreeReader::parsers()
{
  if( _parsers.empty() )
  {
    if( _factory )
      _parsers.push_back( new TreeParser_shfj( *_factory, _syntax,
                                                _helpers ) );
    else
      _parsers.push_back( new TreeParser_shfj( _syntax, _helpers ) );
    _parsers.push_back( new TreeParser_Python( _syntax ) );
  }
  return _parsers;
}


Tree* TreeReader::read()
{
  // cout << "TreeReader Tree*\n";
  const list<TreeParser *>		& pars = parsers();
  list<TreeParser *>::const_iterator	ip, ep = pars.end();
  Tree					*t;
  TreeParser				*p;
  int					eprio = 0, etype = -1;
  string				emsg;
  long					pos = 0;

  if( !_datasource )
    throw file_not_found_error( "no IO source", "" );
  if( !_datasource->isOpen() )
    _datasource->open( DataSource::Read );
  if( !*_datasource )
    {
      if( _datasource->isOpen() && _datasource->eof() )
        throw open_error( _datasource->url() + ": file empty or EOF" );
      io_error::launchErrnoExcept( _datasource->url() );
    }

  for( ip=pars.begin(); ip!=ep; ++ip )
    try
      {
	p = *ip;
        p->attach( _datasource );
        pos = _datasource->at();
	t = (*ip)->read();
	p->detach();
	if( t )
	  return t;
        _datasource->at( pos );
      }
    catch( exception & e )
      {
	io_error::keepExceptionPriority( e, eprio, etype, emsg );
	p->detach();
        _datasource->at( pos );
      }
  io_error::launchExcept( etype, emsg, 
			  string( "Unrecognized format or corrupt file " ) 
			  + name() );
  return 0;
}


void TreeReader::read( Tree & tree )
{
  // cout << "TreeReader::read Tree &\n";
  const list<TreeParser *>		& pars = parsers();
  list<TreeParser *>::const_iterator	ip, ep = pars.end();
  TreeParser				*p;
  int					eprio = 0, etype = -1;
  string				emsg;
  long					pos = 0;

  if( !_datasource )
    throw file_not_found_error( "no IO source", "" );
  if( !_datasource->isOpen() )
    _datasource->open( DataSource::Read );
  if( !*_datasource )
    {
      if( _datasource->isOpen() && _datasource->eof() )
        throw open_error( _datasource->url() + ": file empty or EOF" );
      io_error::launchErrnoExcept( _datasource->url() );
    }

  for( ip=pars.begin(); ip!=ep; ++ip )
    try
    {
      p = *ip;
      p->attach( _datasource );
      pos = _datasource->at();
      (*ip)->read( tree );
      p->detach();
      return;
    }
    catch( exception & e )
    {
      io_error::keepExceptionPriority( e, eprio, etype, emsg );
      p->detach();
      _datasource->at( pos );
    }
  io_error::launchExcept( etype, emsg,
                          string( "Unrecognized format or corrupt file " )
                          + name() );
}


void TreeReader::readTree( Tree* t )
{
  read( *t );
}


void TreeReader::open( const string & filename )
{
  attach( filename );
}


void TreeReader::close()
{
  if( _datasource )
    _datasource->close();
}


void TreeReader::attach( istream & s, int )
{
  _datasource.reset( new IStreamDataSource( s ) );
}


void TreeReader::attach( rc_ptr<DataSource> ds )
{
  _datasource = ds;
}


void TreeReader::attach( const string & filename )
{
  _datasource.reset( new FileDataSource( filename ) );
}


void TreeReader::detach()
{
  _datasource.reset( 0 );
}


string TreeReader::name() const
{
  if( !_datasource )
    return string();
  return _datasource->url();
}


