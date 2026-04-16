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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/stlheader.h>
#include <aims/def/general.h>
#include <aims/vector/vector.h>
#include <cartobase/exception/ioexcept.h>
#include <fstream>
#include <soma-io/datasource/streamdatasource.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <soma-io/reader/itemreader.h>
#include <sys/stat.h>


using namespace aims;
using namespace carto;
using namespace std;


STLHeader::STLHeader( const string & filename ) :
  PythonHeader(),
  _filename( filename )
{
}


STLHeader::~STLHeader()
{
}


bool STLHeader::read( size_t* /* offset */ )
{
  setProperty( "file_type", string( "STL" ) );
  setProperty( "data_type", string( "VOID" ) );
  setProperty( "polygon_dimension", 3 );

  readMesh( true );

  // add meta-info to header
  readMinf( removeExtension( filename() ) + extension() + ".minf" );

  return true;
}


string STLHeader::openMode() const
{
  return string("ascii");
}


bool STLHeader::byteSwapping() const
{
  int	bswap = 0;
  getProperty( "byte_swapping", bswap );
  return (bool)bswap;
}


string STLHeader::filename() const
{
  if ( _filename.length() > 4
      && ( _filename.substr( _filename.length() - 4, 4 ) == ".stl"
           || _filename.substr( _filename.length() - 4, 4 ) == ".STL" ) )
    return _filename;
  else
    return _filename + ".stl";
}


set<string> STLHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".stl" );
  return exts;
}


void STLHeader::readMesh( bool stopWhenIdentified )
{
  std::string fname = filename();

  // cout << "STLHeader::readMesh " << fname << endl;

  std::ifstream istr( fname.c_str(), std::ios::in | std::ios::binary );
  istr.unsetf( std::ios::skipws );
  if( !istr )
    soma::io_error::launchErrnoExcept( fname );

  soma::IStreamDataSource ds( istr, fname );
  std::string l;

  if( !soma::StreamUtil::getline( ds, l ) || ds.eof() || l.length() < 7
      || l.substr( 0, 6 ) != "solid " )
    readMeshBinary( stopWhenIdentified, ds );
  else
    readMeshAscii( stopWhenIdentified, ds );
}


void STLHeader::readMeshAscii( bool stopWhenIdentified, DataSource & ds )
{
  // cout << "STLHeader::readMeshAscii\n";

  ds.at( 0 );
  string l;
  unsigned i;

  soma::StreamUtil::getline( ds, l );
  for( i=6; i<l.length(); ++i )
    if( l[i] != ' ' )
      break;
  string name = l.substr( i, l.length() - i );
  setProperty( "name", name );
  setProperty( "ascii", 1 );
}


void STLHeader::readMeshBinary( bool stopWhenIdentified, DataSource & ds )
{
  // cout << "STLHeader::readMeshBinary\n";

  ds.at( 0 );
  char stl_hdr[80];
  if( ds.readBlock( stl_hdr, 80 ) != 80 )
    throw wrong_format_error( "file too short" );

  uint32_t nface = 0x01020304;
  bool byteswap = ((char *) &nface)[0] != 0x04;
  DefaultItemReader<uint32_t>  dir;
  rc_ptr<ItemReader<uint32_t> > itemr( dir.reader( true, byteswap ) );
  if( itemr->read( ds, &nface, 1 ) != 1 )
    throw wrong_format_error( "file too short" );

  offset_t size = ds.size();
  if( size == 0 )
  {
    struct stat s;
    if( stat( ds.url().c_str(), &s ) == 0 )
      size = s.st_size;
  }

  if( size < nface * 50 + 84 )
    throw wrong_format_error( "file too short" );
  if( size > nface * 50 + 84 )
    cout << "warning, file seems too long: " << size << ", expecting: "
    << nface * 50 + 84 << endl;

  setProperty( "format", "STL" );
  setProperty( "object_type", "Mesh" );
  setProperty( "data_type", "VOID" );
  setProperty( "polygon_dimension", 3 );
  setProperty( "ascii", 0 );
  setProperty( "byte_swapping", int(byteswap) );
  setProperty( "polygon_number", nface );

  unsigned i = 0;
  if( strncmp( &stl_hdr[i], "COLOR=", 6 ) == 0 )
  {
    i += 6;
    vector<float> c( 4, 1. );
    c[0] = (uint8_t) stl_hdr[i++] / 255.f;
    c[1] = (uint8_t) stl_hdr[i++] / 255.f;
    c[2] = (uint8_t) stl_hdr[i++] / 255.f;
    c[3] = (uint8_t) stl_hdr[i++] / 255.f;

    Object mat = Object::value( Dictionary() );
    mat->setProperty( "diffuse", c );
    setProperty( "material", mat );
  }
  if( strncmp( &stl_hdr[i], "MATERIAL=", 9 ) == 0 )
  {
    i += 9;
    vector<float> d( 4, 1. ), s( 4, 1. ), a( 4, 1. );
    d[0] = (uint8_t) stl_hdr[i++] / 255.f;
    d[1] = (uint8_t) stl_hdr[i++] / 255.f;
    d[2] = (uint8_t) stl_hdr[i++] / 255.f;
    d[3] = (uint8_t) stl_hdr[i++] / 255.f;
    s[0] = (uint8_t) stl_hdr[i++] / 255.f;
    s[1] = (uint8_t) stl_hdr[i++] / 255.f;
    s[2] = (uint8_t) stl_hdr[i++] / 255.f;
    s[3] = (uint8_t) stl_hdr[i++] / 255.f;
    a[0] = (uint8_t) stl_hdr[i++] / 255.f;
    a[1] = (uint8_t) stl_hdr[i++] / 255.f;
    a[2] = (uint8_t) stl_hdr[i++] / 255.f;
    a[3] = (uint8_t) stl_hdr[i++] / 255.f;

    Object mat = Object::value( Dictionary() );
    mat->setProperty( "diffuse", d );
    mat->setProperty( "specular", s );
    mat->setProperty( "ambient", a );
    setProperty( "material", mat );
  }

  if( stopWhenIdentified )
    return;

  throw wrong_format_error( "not implemented" );
}
