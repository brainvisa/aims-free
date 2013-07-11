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

#include <aims/io/mniobjheader.h>
#include <aims/def/general.h>
#include <aims/io/byteswap.h>
#include <cartobase/exception/ioexcept.h>
#ifdef USE_SOMA_IO
  #include <soma-io/utilities/asciidatasourcetraits.h>
  #include <soma-io/datasource/streamdatasource.h>
#else
  #include <cartobase/io/asciidatasourcetraits.h>
  #include <cartobase/datasource/streamdatasource.h>
#endif
#include <cartobase/type/string_conversion.h>
#include <cartobase/stream/fileutil.h>
#include <fstream>

using namespace aims;
using namespace carto;
using namespace std;


MniObjHeader::MniObjHeader( const string & filename ) :
  PythonHeader(),
  _filename( filename )
{
}


MniObjHeader::~MniObjHeader()
{
}


bool MniObjHeader::read( uint32_t* offset )
{
  string    fname = filename();

  if( FileUtil::fileStat( fname ).find( '+' ) == string::npos )
    throw file_not_found_error( fname );
  ifstream  is( fname.c_str(), ios::in | ios::binary );
  if( !is )
    io_error::launchErrnoExcept( fname );
  is.unsetf( ios::skipws );

  uint32_t  nvertex = 0;
  bool    ascii;
  char    c = ' ';
  set<char> spaces;
  spaces.insert( ' ' );
  spaces.insert( '\n' );
  spaces.insert( '\t' );
  spaces.insert( '\r' );
  set<char>::const_iterator isp, nospace = spaces.end();
  set<string> sspaces;
  for( isp=spaces.begin(); isp!=nospace; ++isp )
    sspaces.insert( string() + *isp );

  while( spaces.find( c ) != nospace )
    c = is.get();
  if( !is )
    io_error::launchErrnoExcept( fname );
  if( c != 'P' )
    throw wrong_format_error( fname );
  ascii = true;
  c = is.get();
  if( spaces.find( c ) == nospace )
    throw wrong_format_error( fname );

  IStreamDataSource ds( is, fname );
  string l;
  if( !StreamUtil::getline( ds, l ) )
    throw wrong_format_error( fname );
  vector<string> lspl = split( l, sspaces );
  if( lspl.size() != 6 )
    throw wrong_format_error( fname );
  float nf;
  for( int i=0; i<3; ++i )
    stringTo( lspl[i], nf );
  int n;
  for( int i=3; i<6; ++i )
    stringTo( lspl[i], n );
  nvertex = n;
  // cout << "vertices: " << nvertex << endl;
  if( nvertex == 0 )
    throw wrong_format_error( fname );

  if ( offset )
    *offset = is.tellg();
  is.close();
  setProperty( "file_type", string( "MNI_OBJ" ) );
  setProperty( "data_type", "VOID" );
  setProperty( "ascii", (int) ascii );
/*  if( !ascii )
    setProperty( "byte_swapping",
      (int) ( magicNumber != AIMS_MAGIC_NUMBER ) );*/
  setProperty( "polygon_dimension", (int) 3 );
  setProperty( "nb_t_pos", (int) 1 );
  setProperty( "vertex_number", (int) nvertex );

  // add meta-info to header
  readMinf( removeExtension( fname ) + extension() + ".minf" );

  return true;
}


string MniObjHeader::openMode() const
{
  int om = 0;
  getProperty( "ascii", om );
  if ( om )
    return string("ascii");
  return string("binar");
}


bool MniObjHeader::byteSwapping() const
{
  int bswap = 0;
  getProperty( "byte_swapping", bswap );
  return bswap ? true : false;
}


string MniObjHeader::filename() const
{
  if( _filename.length() > 5
      && _filename.substr( _filename.length()-5, 5 ) == ".obj" )
    return _filename;
  else
  {
    if( FileUtil::fileStat( _filename ).find( 'r' ) != string::npos )
      return _filename;
    return _filename + ".obj";
  }
}


set<string> MniObjHeader::extensions() const
{
  set<string> exts;
  exts.insert( ".obj" );
  return exts;
}

