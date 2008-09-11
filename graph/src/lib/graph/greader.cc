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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/graph/greader.h>
#include <graph/graph/gparser_shfj.h>
#include <graph/graph/gparser_xml.h>
#include <graph/graph/gparser_python.h>
#include <graph/graph/graph.h>
#include <cartobase/object/lexicon.h>
#include <cartobase/exception/parse.h>
#include <cartobase/datasource/streamdatasource.h>
#include <cartobase/datasource/filedatasource.h>

using namespace carto;
using namespace std;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphReader::GraphReader(const std::string& filename, 
			 const SyntaxSet& syntax,
			 const AttributedReader::HelperSet& helpers)
  :_parser(-1),
   _syntax(syntax),
   _helpers(helpers), 
   _datasource( new FileDataSource( filename ) )
{
}


GraphReader::GraphReader(const SyntaxSet& syntax,
			 const AttributedReader::HelperSet& helpers)
  :_parser(-1),
   _syntax(syntax),
   _helpers(helpers)
{
}


GraphReader::~GraphReader()
{
  unsigned	i, n = _parsers.size();
  for( i=0; i<n; ++i )
    delete _parsers[i];
}


void
GraphReader::read(Graph& graph)
{
  int					i, n = _parsers.size(), res;
  int					eprio = 0, etype = -1;
  string				emsg;

  if( !_datasource )
    throw file_not_found_error( "no IO source", "" );
  if( !_datasource->isOpen() )
    _datasource->open( DataSource::Read );
  if( !*_datasource )
    io_error::launchErrnoExcept( _datasource->url() );

  long			pos = 0;

  // cout << "GraphReader: filename: " << _datasource->url() << endl;
  _parser = -1;
  for( i=0; i<n; ++i )
    {
      GraphParser	& gp = *_parsers[i];
      try
        {
          gp.attach( _datasource );
          pos = _datasource->at();
          res = gp.read( graph );
          gp.detach();
          if( res == 0 )
            {
              _parser = i;
              return;
            }
          _datasource->at( pos );
        }
      catch( exception & e )
        {
	  io_error::keepExceptionPriority( e, eprio, etype, emsg );
          gp.detach();
          _datasource->at( pos );
        }
    }

  vector<int>	tryformats;
  map<string,int>::iterator	ip, ep = _parsernames.end();
  ip = _parsernames.find( "xml" );
  if( ip == ep )
    {
      i = _parsers.size();
      _parsers.push_back( new GraphParser_xml );
      _parsernames[ "xml" ] = i;
      tryformats.push_back( i );
    }
  ip = _parsernames.find( "shfj" );
  if( ip == ep )
    {
      i = _parsers.size();
      _parsers.push_back( new GraphParser_shfj( _syntax, _helpers ) );
      _parsernames[ "shfj" ] = i;
      tryformats.push_back( i );
    }
  ip = _parsernames.find( "python" );
  if( ip == ep )
    {
      i = _parsers.size();
      _parsers.push_back( new GraphParser_Python( _syntax ) );
      _parsernames[ "python" ] = i;
      tryformats.push_back( i );
    }

  for( i=0, n=tryformats.size(); i<n; ++i )
    {
      GraphParser	& gp = *_parsers[ tryformats[i] ];
      try
        {
          gp.attach( _datasource );
          pos = _datasource->at();
          res = gp.read( graph );
          gp.detach();
          if( res == 0 )
            {
              _parser = i;
              return;
            }
          _datasource->at( pos );
        }
      catch( exception & e )
        {
	  io_error::keepExceptionPriority( e, eprio, etype, emsg );
          gp.detach();
          _datasource->at( pos );
        }
    }
  io_error::launchExcept( etype, emsg );
}


void
GraphReader::open(const std::string& filename)
{
  attach( filename );
}


void
GraphReader::close()
{
  if( _datasource )
    _datasource->close();
}

void
GraphReader::attach(std::istream &s, int /* line_num */ )
{
  // filtering DS
  _datasource.reset( new IStreamDataSource( s ) );
}


void
GraphReader::attach( rc_ptr<DataSource> ds )
{
  // filtering DS
  _datasource = ds;
}


void
GraphReader::attach( const std::string & filename )
{
  // filtering DS
  _datasource.reset( new FileDataSource( filename ) );
}


int
GraphReader::line() const
{
  return 0;
}


std::string
GraphReader::name() const
{
  if( !_datasource )
    return string();
  return _datasource->url();
}


bool
GraphReader::operator!() const
{
  return !_datasource || !*_datasource;
}


bool
GraphReader::is_open() const
{
  return _datasource && _datasource->isOpen();
}


bool
GraphReader::eof() const
{
  return !_datasource || _datasource->eof();
}


rc_ptr<DataSource> GraphReader::dataSource()
{
  return _datasource;
}


const rc_ptr<DataSource> GraphReader::dataSource() const
{
  return _datasource;
}


GraphReader&
operator>>(GraphReader& reader, Graph& graph)
{
  reader.read(graph);
  return reader;
}
