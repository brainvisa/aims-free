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

#include <aims/io/argheader.h>
#include <aims/def/general.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/defaultItemW.h>
#include <aims/fibers/bundles.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/streamdatasource.h>

using namespace aims;
using namespace carto;
using namespace std;


string ArgHeader::filename() const
{
  if( _name.length() >= 4 && _name.substr( _name.length() - 4, 4 ) == ".arg" )
    return( _name );
  if( _name.length() >= 8 && 
      _name.substr( _name.length() - 8, 8 ) == ".bundles" )
    return( _name );
  if( _name.length() >= 4 &&
      _name.substr( _name.length() - 4, 4 ) == ".trk" )
    return( _name );
  if( _name.length() >= 4 &&
      _name.substr( _name.length() - 4, 4 ) == ".tck" )
    return( _name );
  return( _name + ".arg" );
}


void ArgHeader::read( size_t * )
{
  string fileName = filename();
  if( ( fileName.length() >= 8 &&
        fileName.substr( _name.length() - 8, 8 ) == ".bundles" )
      || ( fileName.length() >= 4 &&
           fileName.substr( _name.length() - 4, 4 ) == ".trk" )
      || ( fileName.length() >= 4 &&
           fileName.substr( _name.length() - 4, 4 ) == ".tck" ) )
  {
    BundleReader bundelReader( fileName );
    Object hdr = bundelReader.readHeader();

    if( hdr )
      copyProperties( hdr );

    setProperty( "file_type", string( "BUNDLES" ) );
    setProperty( "object_type", string( "Graph" ) );
    setProperty( "data_type", string( "VOID" ) );
    // add meta-info to header
    readMinf( fileName + ".minf" );
  }
  else
  {
    // ARG format
    ifstream is( fileName.c_str() );
    if( !is )
      io_error::launchErrnoExcept( fileName );
    IStreamDataSource ds( is, fileName );

    setProperty( "file_type", string( "ARG" ) );
    setProperty( "object_type", string( "Graph" ) );
    setProperty( "data_type", string( "VOID" ) );

    // try to read syntactic attribute
    string	l;
    char		c;

    do {
      l.erase( 0, l.length() );
      while( !ds.eof() )
      {
        c = ds.getch();
        if( c != '\n' && c != '\0' && c != '\r' )
          l += c;
        else
          break;
      }
    } while( !ds.eof() && ( l.length() == 0 || l[0] == '#' ) );
    if( l.find( "*BEGIN GRAPH" ) == 0 )
    {
      unsigned	i = 12;
      while( l.length() > i && ( l[i] == ' ' || l[i] == '\t' ) )
        ++i;
      l.erase( 0, i );
      setProperty( "arg_syntax", l );
    }
    else
      throw wrong_format_error( fileName );
    // add meta-info to header
    readMinf( removeExtension( _name ) + extension() + ".minf" );
  }
}


void ArgHeader::write()
{
  string fileName = filename();
  ofstream os( fileName.c_str() );
  if( !os )
    io_error::launchErrnoExcept( fileName );

// Yann // To check and complete

  // write meta-info header
  setProperty( "file_type", string( "ARG" ) );
  setProperty( "object_type", string( "Graph" ) );
  setProperty( "data_type", string( "VOID" ) );

  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}


set<string> ArgHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".arg" );
  exts.insert( ".bundles" );
  exts.insert( ".trk" );
  exts.insert( ".tck" );
  return exts;
}
