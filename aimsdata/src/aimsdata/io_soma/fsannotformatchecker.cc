#include <aims/io_soma/fsannotformatchecker.h>
#include <aims/io/finder.h>
#include <aims/io/byteswap.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/object/property.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/reader/itemreader.h>


using namespace aims;
using namespace soma;
using namespace std;

// from https://surfer.nmr.mgh.harvard.edu/fswiki/LabelsClutsAnnotationFiles

DataSourceInfo FsAnnotFormatChecker::check( DataSourceInfo dsi,
                                            DataSourceInfoLoader &,
                                            carto::Object options )
                                            const
{
  // cout << "FsAnnotFormatChecker::check\n";

  bool dolist = dsi.list().typecount() == 1;
  if( dolist )
  {
    DataSource* pds = dsi.list().dataSource().get();

    dsi.list().addDataSource( "annot", rc_ptr<DataSource>( pds ) );

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

    rc_ptr<DataSource> ds = dsi.list().dataSource( "annot" );
    if( !ds->isOpen() )
      ds->open( DataSource::Read );
    if( !ds->isOpen() )
      throw file_not_found_error( ds->url() );

    Object hdr = Object::value( PropertySet() );  // header
    bool res = checkBinaryTexture( ds, hdr );

    if( !res )
      throw wrong_format_error( ds->url() );

    hdr->setProperty( "format", "FSANNOT" );
    hdr->setProperty( "object_type", "Texture" );
    hdr->setProperty( "data_type", "S16" );

    Object tprop0 = Object::value( Dictionary() );
    tprop0->setProperty( "interpolation", "rgb" );
    vector<Object> tprop;
    tprop.push_back( tprop0 );
    hdr->setProperty( "texture_properties", tprop );

    dsi.header() = hdr;
  }

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    dsi.capabilities().setDataSource( dsi.list().dataSource( "annot" ) );
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


bool FsAnnotFormatChecker::checkBinaryTexture( rc_ptr<DataSource> ds,
                                               Object hdr ) const
{
  int32_t nv = 1;
  bool bswap = ( *( (char *) &nv ) == 1 );
  nv = 0;

  DefaultItemReader<int32_t>  dir;
  std::unique_ptr<ItemReader<int32_t> > itemr;
  itemr.reset( dir.reader( true, bswap ) );

  if( itemr->read( *ds, &nv, 1 ) != 1 || !ds->isOpen()
      || ds->eof() )
    return false;

  if( nv < 0 || nv > 100000000 ) // bad test
    return false;

  offset_t fsize = ds->size();
  if( fsize < nv * 8 + 8 || fsize > nv * 8 + 10000 )
    return false;

  int32_t v0;
  if( itemr->read( *ds, &v0, 1 ) != 1 || !ds->isOpen()
      || ds->eof() )
    return false;

  if( v0 != 0 )  // should start at vertex index 0
    return false;
  ds->ungetch( 0 );
  ds->ungetch( 0 );
  ds->ungetch( 0 );
  ds->ungetch( 0 );

  hdr->setProperty( "vertex_number", nv );
  hdr->setProperty( "byte_swapping", int(bswap) );

  return true;
}


