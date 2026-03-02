#include <aims/io/baseFormats_mesh.h>
#include <aims/io/stlheader.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/fileutil.h>

#include <soma-io/datasource/streamdatasource.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/reader/itemreader.h>
#include <soma-io/writer/itemwriter.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/stringutil.h>


using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  struct Point3dfCompare
    : public std::binary_function<Point3df,Point3df, bool>
  {
    bool operator () ( const Point3df & p1, const Point3df & p2 ) const
    {
      return p1[2] < p2[2] ||
        ( p1[2] == p2[2] &&
          ( p1[1] < p2[1] ||
        ( p1[1] == p2[1] &&
          ( p1[0] < p2[0] ) ) ) );
    }
  };

}


STLMeshFormat::~STLMeshFormat()
{
}


bool STLMeshFormat::write( const std::string & filename,
                           const AimsTimeSurface<3, Void> & thing,
                           carto::Object options )
{
  bool ascii = false;
  try
  {
    Object o = options->getProperty( "ascii" );
    ascii = bool( o->getScalar() );
  }
  catch( ... )
  {
  }
  if( ascii )
    return writeAscii( filename, thing, options );
  return writeBinary( filename, thing, options );
}


bool STLMeshFormat::writeAscii( const std::string & filename,
                                const AimsTimeSurface<3, Void> & thing,
                                carto::Object options )
{
  // cout << "STLMeshFormat::writeascii " << filename << endl;
  ofstream ostr( filename.c_str(), ios::out );
  ostr.unsetf( std::ios::skipws );
  if( !ostr )
    soma::io_error::launchErrnoExcept( filename );

  vector<AimsVector<uint, 3> >::const_iterator ip, ep = thing.polygon().end();
  const vector<Point3df> & vertex = thing.vertex();
  const vector<Point3df> & normal = thing.normal();
  Point3df n;

  ostr << "solid " << filename << endl;
  for( ip=thing.polygon().begin(); ip!=ep; ++ip )
  {
    n = ( normal[(*ip)[0]] + normal[(*ip)[1]] + normal[(*ip)[2]] ) / 3;
    const Point3df & v1 = vertex[(*ip)[0]];
    const Point3df & v2 = vertex[(*ip)[1]];
    const Point3df & v3 = vertex[(*ip)[2]];
    ostr << "facet normal " << n[0] << " " << n[1] << " " << n[2] << endl;
    ostr << "    outer loop\n";
    ostr << "        vertex " << v1[0] << " " << v1[1] << " " << v1[2] << endl;
    ostr << "        vertex " << v2[0] << " " << v2[1] << " " << v2[2] << endl;
    ostr << "        vertex " << v3[0] << " " << v3[1] << " " << v3[2] << endl;
    ostr << "    endloop\n";
    ostr << "endfacet\n";
  }
  ostr << "endsolid " << filename << endl;

  return true;
}


bool STLMeshFormat::writeBinary( const std::string & filename,
                                 const AimsTimeSurface<3, Void> & thing,
                                 carto::Object options )
{
  // cout << "STLMeshFormat::writeBinary " << filename << endl;
  uint32_t nface = 0x01020304;
  bool byteswap = ((char *) &nface)[0] != 0x04;
  DefaultItemWriter<uint32_t>  dir;
  rc_ptr<ItemWriter<uint32_t> > itemw( dir.writer( true, byteswap ) );
  DefaultItemWriter<float>  df;
  rc_ptr<ItemWriter<float> > fitemw( df.writer( true, byteswap ) );
  DefaultItemWriter<uint16_t>  dsi;
  rc_ptr<ItemWriter<uint16_t> > sitemw( dsi.writer( true, byteswap ) );

  ofstream ostr( filename.c_str(), ios::out | ios::binary );
  ostr.unsetf( std::ios::skipws );
  if( !ostr )
    soma::io_error::launchErrnoExcept( filename );

  soma::OStreamDataSource ds( ostr );

  vector<char> buf_hdr( 80, 0 );
  try
  {
    Object m = thing.header().getProperty( "material" );
    if( m->hasProperty( "diffuse" ) || m->hasProperty( "ambient" )
        || m->hasProperty( "specular" ) )
    {
      AimsRGBA d( 204, 204, 204, 255 ), s( 51, 51, 51, 255 ),
        a( 25, 25, 25, 255 );
      try
      {
        Object c = m->getProperty( "diffuse" );
        d[0] = int( c->getArrayItem( 0 )->getScalar() * 255.9 );
        d[1] = int( c->getArrayItem( 1 )->getScalar() * 255.9 );
        d[2] = int( c->getArrayItem( 2 )->getScalar() * 255.9 );
        d[3] = int( c->getArrayItem( 3 )->getScalar() * 255.9 );
      }
      catch( ... )
      {
      }
      try
      {
        Object c = m->getProperty( "specular" );
        s[0] = int( c->getArrayItem( 0 )->getScalar() * 255.9 );
        s[1] = int( c->getArrayItem( 1 )->getScalar() * 255.9 );
        s[2] = int( c->getArrayItem( 2 )->getScalar() * 255.9 );
        s[3] = int( c->getArrayItem( 3 )->getScalar() * 255.9 );
      }
      catch( ... )
      {
      }
      try
      {
        Object c = m->getProperty( "ambient" );
        a[0] = int( c->getArrayItem( 0 )->getScalar() * 255.9 );
        a[1] = int( c->getArrayItem( 1 )->getScalar() * 255.9 );
        a[2] = int( c->getArrayItem( 2 )->getScalar() * 255.9 );
        a[3] = int( c->getArrayItem( 3 )->getScalar() * 255.9 );
      }
      catch( ... )
      {
      }
      buf_hdr[0] = 'M';
      buf_hdr[1] = 'A';
      buf_hdr[2] = 'T';
      buf_hdr[3] = 'E';
      buf_hdr[4] = 'R';
      buf_hdr[5] = 'I';
      buf_hdr[6] = 'A';
      buf_hdr[7] = 'L';
      buf_hdr[8] = '=';
      buf_hdr[9] = d[0];
      buf_hdr[10] = d[1];
      buf_hdr[11] = d[2];
      buf_hdr[12] = d[3];
      buf_hdr[13] = s[0];
      buf_hdr[14] = s[1];
      buf_hdr[15] = s[2];
      buf_hdr[16] = s[3];
      buf_hdr[17] = a[0];
      buf_hdr[18] = a[1];
      buf_hdr[19] = a[2];
      buf_hdr[20] = a[3];
    }
  }
  catch( ... )
  {
  }
  ds.writeBlock( &buf_hdr[0], 80 );

  uint32_t npoly = uint32_t( thing.polygon().size() );
  itemw->write( ds, npoly );

  vector<AimsVector<uint, 3> >::const_iterator ip, ep = thing.polygon().end();
  const vector<Point3df> & vertex = thing.vertex();
  const vector<Point3df> & normal = thing.normal();
  Point3df n;

  for( ip=thing.polygon().begin(); ip!=ep; ++ip )
  {
    n = ( normal[(*ip)[0]] + normal[(*ip)[1]] + normal[(*ip)[2]] ) / 3;
    fitemw->write( ds, n[0] );
    fitemw->write( ds, n[1] );
    fitemw->write( ds, n[2] );
    const Point3df & v1 = vertex[(*ip)[0]];
    fitemw->write( ds, v1[0] );
    fitemw->write( ds, v1[1] );
    fitemw->write( ds, v1[2] );
    const Point3df & v2 = vertex[(*ip)[1]];
    fitemw->write( ds, v2[0] );
    fitemw->write( ds, v2[1] );
    fitemw->write( ds, v2[2] );
    const Point3df & v3 = vertex[(*ip)[2]];
    fitemw->write( ds, v3[0] );
    fitemw->write( ds, v3[1] );
    fitemw->write( ds, v3[2] );
    sitemw->write( ds, 0 );
  }

  return true;
}


bool STLMeshFormat::read( const string & filename,
                          AimsTimeSurface<3, Void>& thing,
                          const carto::AllocatorContext & context,
                          carto::Object options )
{
  STLHeader hdr( filename );
  hdr.read();

  int ascii = false;
  hdr.getProperty( "ascii", ascii );
  if( ascii )
    return readAscii( filename, thing, context, options, hdr );
  return readBinary( filename, thing, context, options, hdr );
}


bool STLMeshFormat::readAscii( const string & filename,
                               AimsTimeSurface<3, Void>& thing,
                               const carto::AllocatorContext & /*context*/,
                               carto::Object /*options*/,
                               PythonHeader & hdr )
{
  cout << "STLMeshFormat::readAscii " << filename << endl;

  ifstream istr( filename.c_str(), ios::in );
  if( !istr )
    soma::io_error::launchErrnoExcept( filename );

  soma::IStreamDataSource ds( istr );
  thing.erase();
  string l;

  soma::StreamUtil::getline( ds, l );
  Point3df n, v;
  map<Point3df, uint32_t, Point3dfCompare> vindex;
  map<Point3df, uint32_t, Point3dfCompare>::iterator ivi;
  AimsVector<uint, 3> p;
  uint32_t nv = 0, iv, i;
  vector<Point3df> vertices;
  vector<AimsVector<uint, 3> > polygons;

  while( !ds.eof() )
  {
    if( !soma::StreamUtil::getline( ds, l ) || ds.eof() )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l );
    if( l.length() >= 8 && l.substr( 0, 8 ) == "endsolid" )
      break;
    if( l.length() < 13 || l.substr( 0, 6 ) != "facet " )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l.substr( 6, l.length() - 7 ) );
    if( l.substr( 0, 7 ) != "normal " )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l.substr( 7, l.length() - 7 ) );
    stringstream s( l );
    s >> n[0] >> n[1] >> n[2];

    if( !soma::StreamUtil::getline( ds, l ) || ds.eof() )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l );
    if( l.length() < 10 || l.substr( 0, 6 ) != "outer " )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l.substr( 6, l.length() - 6 ) );
    if( l != "loop" )
      throw wrong_format_error( "file too short" );

    for( i=0; i<3; ++i )
    {
      if( !soma::StreamUtil::getline( ds, l ) || ds.eof() )
        throw wrong_format_error( "file too short" );
      l = StringUtil::strip( l );
      if( l.length() < 8 || l.substr( 0, 7 ) != "vertex " )
        throw wrong_format_error( "file too short" );
      l = StringUtil::strip( l.substr( 7, l.length() - 7 ) );
      stringstream s( l );
      s >> v[0] >> v[1] >> v[2];
      ivi = vindex.find( v );
      if( ivi == vindex.end() )
      {
        vindex[v] = nv;
        vertices.push_back( v );
        iv = nv++;
      }
      else
        iv = ivi->second;
      p[i] = iv;
    }
    polygons.push_back( p );

    if( !soma::StreamUtil::getline( ds, l ) || ds.eof() )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l );
    if( l != "endloop" )
      throw wrong_format_error( "file too short" );
    if( !soma::StreamUtil::getline( ds, l ) || ds.eof() )
      throw wrong_format_error( "file too short" );
    l = StringUtil::strip( l );
    if( l != "endfacet" )
      throw wrong_format_error( "file too short" );
  }

  thing.vertex() = vertices;
  thing.polygon() = polygons;
  // thing.normal() = normals;

  thing.updateNormals();
  hdr.setProperty( "vertex_number", vertices.size() );
  thing.header().copy( hdr );

  return true;
}


bool STLMeshFormat::readBinary( const string & filename,
                                AimsTimeSurface<3, Void>& thing,
                                const carto::AllocatorContext & /*context*/,
                                carto::Object /*options*/,
                                PythonHeader & hdr )
{
  ifstream istr( filename.c_str(), ios::in | ios::binary );
  istr.unsetf( std::ios::skipws );
  if( !istr )
    soma::io_error::launchErrnoExcept( filename );

  soma::IStreamDataSource ds( istr );
  thing.erase();
  std::string l;

  char stl_hdr[80];
  if( ds.readBlock( stl_hdr, 80 ) != 80 )
    throw wrong_format_error( "file too short" );

  uint32_t nface = 0x01020304;
  bool byteswap = ((char *) &nface)[0] != 0x04;
  DefaultItemReader<uint32_t>  dir;
  rc_ptr<ItemReader<uint32_t> > itemr( dir.reader( true, byteswap ) );
  if( itemr->read( ds, &nface, 1 ) != 1 )
    throw wrong_format_error( "file too short" );

  DefaultItemReader<float>  df;
  rc_ptr<ItemReader<float> > fitemr( df.reader( true, byteswap ) );
  DefaultItemReader<uint16_t>  dsi;
  rc_ptr<ItemReader<uint16_t> > sitemr( dsi.reader( true, byteswap ) );

  vector<Point3df> vertices, normals;
  vector<AimsVector<uint, 3> > polygons;
  polygons.reserve( nface );
  uint32_t poly, nv = 0, iv, i;
  Point3df n, v;
  map<Point3df, uint32_t, Point3dfCompare> vindex;
  map<Point3df, uint32_t, Point3dfCompare>::iterator ivi;
  AimsVector<uint, 3> p;
  uint16_t abc;

  for( poly=0; poly<nface; ++poly )
  {
    if( ds.eof() )
      throw eof_error( filename );
    if( fitemr->read( ds, &n[0], 3 ) != 3 )
      throw eof_error( filename );
    for( i=0; i<3; ++i )
    {
      if( fitemr->read( ds, &v[0], 3 ) != 3 )
        throw eof_error( filename );
      ivi = vindex.find( v );
      if( ivi == vindex.end() )
      {
        vindex[v] = nv;
        vertices.push_back( v );
        iv = nv++;
      }
      else
        iv = ivi->second;
      p[i] = iv;
    }
    polygons.push_back( p );
    sitemr->read( ds, &abc, 1 );  // "attribute byte count, should be 0
  }

  thing.vertex() = vertices;
  thing.polygon() = polygons;
  thing.normal() = normals;

  thing.updateNormals();
  hdr.setProperty( "vertex_number", vertices.size() );
  thing.header().copy( hdr );

  return true;
}

