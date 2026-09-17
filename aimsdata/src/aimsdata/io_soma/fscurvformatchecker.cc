#include <aims/io_soma/fscurvformatchecker.h>
#include <aims/io/byteswap.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/object/property.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/reader/itemreader.h>


using namespace aims;
using namespace soma;
using namespace std;

// from http://www.grahamwideman.com/gw/brain/fs/surfacefileformats.htm

DataSourceInfo FsCurvFormatChecker::check( DataSourceInfo dsi,
                                        DataSourceInfoLoader &,
                                        carto::Object options )
                                        const
{
  // cout << "FsCurvFormatChecker::check\n";

  bool dolist = dsi.list().typecount() == 1;
  if( dolist )
  {
    DataSource* pds = dsi.list().dataSource().get();

    dsi.list().addDataSource( "curv", rc_ptr<DataSource>( pds ) );

    string url = FileUtil::uriFilename( pds->url() );
    string minfname = url + ".minf";

    //// Minf DataSource
    if( FileUtil::fileStat( minfname ).find( '+' ) != string::npos )
    {
      // if minf file exist
      dsi.list().addDataSource(
        "minf", rc_ptr<DataSource>( new FileDataSource( minfname ) ) );
    }
  }

  bool doread = dsi.header().isNone();
  if( doread )
  {
    // string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );
    // cout << "URL: " << url << endl;

    rc_ptr<DataSource> ds = dsi.list().dataSource( "curv" );
    if( !ds->isOpen() )
      ds->open( DataSource::Read );
    if( !ds->isOpen() )
      throw file_not_found_error( ds->url() );

    Object hdr = Object::value( PropertySet() );  // header
    bool res = checkBinaryTexture( ds, hdr );

    if( !res )
      throw wrong_format_error( ds->url() );

    hdr->setProperty( "format", "FSCURV" );
    hdr->setProperty( "object_type", "Texture" );
    hdr->setProperty( "data_type", "FLOAT" );

    dsi.header() = hdr;
  }

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    dsi.capabilities().setDataSource( dsi.list().dataSource( "curv" ) );
    dsi.capabilities().setThreadSafe( true );
    dsi.capabilities().setOrdered( true );
    dsi.capabilities().setSeekVoxel( false );
    dsi.capabilities().setSeekLine( false );
    dsi.capabilities().setSeekSlice( false );
    dsi.capabilities().setSeekVolume( false );
    dsi.capabilities().setRandomAccessEfficient( false );
    dsi.capabilities().setHandleStrides( false );
  }

  return dsi;
}


bool FsCurvFormatChecker::checkAsciiTexture( rc_ptr<DataSource> ds,
                                             Object hdr ) const
{
  return false;
}


bool FsCurvFormatChecker::checkBinaryTexture( rc_ptr<DataSource> ds,
                                              Object hdr ) const
{
  uint32_t magic = 1;
  bool bswap = ( *( (char *) &magic ) == 1 );
  int offset = 0;
  offset = 1;
  magic = 0;

  if( ds->readBlock( ((char *) &magic) + offset, 3 ) != 3 || ds->eof() )
    return false;
  if( bswap )
    magic = byteswap( magic );

  DefaultItemReader<int32_t>  dir;
  std::unique_ptr<ItemReader<int32_t> > itemr;
  if( magic == 0xffffff )
  {
    itemr.reset( dir.reader( true, bswap ) );
    vector<int32_t> nv( 3, 0 );
    if( itemr->read( *ds, &nv[0], 3 ) != 3 || !ds->isOpen()
        || ds->eof() )
      return false;
    hdr->setProperty( "vertex_number", nv[0] );
    hdr->setProperty( "polygon_number", nv[1] );
    hdr->setProperty( "texture_dimension", nv[2] );
  }
  else
  {
    // cout << "not a FS texture file: magic: " << magic << endl;
    return false;
  }
  hdr->setProperty( "byte_swapping", int(bswap) );

  return true;
}


