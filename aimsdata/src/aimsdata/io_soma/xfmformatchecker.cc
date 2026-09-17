#include <aims/io_soma/xfmformatchecker.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/finder.h>
#include <aims/data/pheader.h>
#include <aims/io_soma/xfmformatreader.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/datasource/filedatasource.h>


using namespace aims;
using namespace soma;
using namespace std;

DataSourceInfo XfmFormatChecker::check( DataSourceInfo dsi,
                                        DataSourceInfoLoader &,
                                        carto::Object options )
                                        const
{
  // cout << "XfmFormatChecker::check\n";

  bool dolist = dsi.list().typecount() == 1;
  if( dolist )
  {
    DataSource* pds = dsi.list().dataSource().get();

    dsi.list().addDataSource( "xfm", rc_ptr<DataSource>( pds ) );

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
    string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );
    // cout << "URL: " << url << endl;

    // simple test: try to read all...
    XfmFormatReader mr;
    AffineTransformation3d m;
    rc_ptr<DataSourceInfo> rdsi( &dsi );
    try
    {
      mr.read( m, rdsi, AllocatorContext(), options );
    }
    catch( ... )
    {
      rdsi.release();
      throw;
    }

    rdsi.release();

    Object hdr = Object::value( PropertySet() );  // header

    hdr->copyProperties( m.header() );

    hdr->setProperty( "format", "XFM" );
    hdr->setProperty( "object_type", "AffineTransformation3d" );
    hdr->setProperty( "data_type", "VOID" );

    dsi.header() = hdr;
  }

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    dsi.capabilities().setDataSource( dsi.list().dataSource( "xfm" ) );
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

