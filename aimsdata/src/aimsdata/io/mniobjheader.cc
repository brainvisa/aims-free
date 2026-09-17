// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/mniobjheader.h>
#include <aims/def/general.h>
#include <aims/io/byteswap.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <soma-io/datasource/streamdatasource.h>
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

