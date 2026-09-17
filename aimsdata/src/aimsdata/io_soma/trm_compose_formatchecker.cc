#include <aims/io_soma/trm_compose_formatchecker.h>
#include <aims/io/finder.h>
#include <aims/data/pheader.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/stringutil.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>


using namespace aims;
using namespace carto;
using namespace soma;
using namespace std;

DataSourceInfo TrmComposeFormatChecker::check( DataSourceInfo dsi,
                                               DataSourceInfoLoader &,
                                               carto::Object options )
                                               const
{
  // cout << "TrmComposeFormatChecker::check\n";

  string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );
  if( url.length() > 5 && url.substr( url.length() - 5, 5 ) == ".trmc" )
    url = url.substr( 0, url.length() - 5 );
  else
    throw wrong_format_error( "Not a transform-compose",
                              dsi.list().dataSource()->url() );

  vector<string> urls = StringUtil::split( url, "*" );
  Finder f;

  vector<string>::const_iterator iu, eu = urls.end();
  vector<string> chained;
  chained.reserve( urls.size() );

  for( iu=urls.begin(); iu!=eu; ++iu )
  {
    string url = StringUtil::strip( *iu );
    if( !f.check( url ) )
      throw wrong_format_error( "Not a transform", url );

    const PythonHeader *ph = dynamic_cast<const PythonHeader *>( f.header() );
    if( !ph )
      throw wrong_format_error( "Not a transform", url );
    if( ph->getProperty( "object_type" )->getString()
        != "AffineTransformation3d" )
      throw wrong_format_error( "Not a transform", url );
    chained.push_back( url );
  }

  Object hdr = Object::value( PropertySet() );  // header

  hdr->setProperty( "format", "TRMCOMPOSE" );
  hdr->setProperty( "object_type", "AffineTransformation3d" );
  hdr->setProperty( "data_type", "VOID" );
  hdr->setProperty( "transformation_chain", chained );

  dsi.header() = hdr;

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    int i;
    for( i=0, iu=chained.begin(), eu=chained.end(); iu!=eu; ++iu, ++i )
    {
      dsi.list().addDataSource(
        "trm_chain", rc_ptr<DataSource>( new FileDataSource( *iu ) ) );
    }
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


DataSourceInfo TrmChainFormatChecker::check( DataSourceInfo dsi,
                                             DataSourceInfoLoader &,
                                             carto::Object options )
                                             const
{
  // cout << "TrmChainFormatChecker::check " << dsi.url() << endl;

  string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );
  if( url.length() > 5 && url.substr( url.length() - 5, 5 ) == ".trmc" )
    url = url.substr( 0, url.length() - 5 );
  else
    throw wrong_format_error( "Not a transform-compose",
                              dsi.list().dataSource()->url() );

  vector<string> urls = StringUtil::split( url, "*" );
  Finder f;

  vector<string>::const_iterator iu, eu = urls.end();
  vector<string> chained;
  chained.reserve( urls.size() );

  for( iu=urls.begin(); iu!=eu; ++iu )
  {
    string url = StringUtil::strip( *iu );
    if( !f.check( url ) )
      throw wrong_format_error( "Not a transform", url );

    const PythonHeader *ph = dynamic_cast<const PythonHeader *>( f.header() );
    if( !ph )
      throw wrong_format_error( "Not a transform", url );
    string ttype = ph->getProperty( "object_type" )->getString();

    bool ok = false;
    if( ttype == "AffineTransformation3d" )
      ok = true;
    else if( ( ttype == "Volume" || ttype != "CartoVolume" )
             && f.dataType() == "POINT3DF" )
      ok = true;
    if( !ok )
      throw wrong_format_error( "Not a transform", url );
    chained.push_back( url );
  }

  Object hdr = Object::value( PropertySet() );  // header

  hdr->setProperty( "format", "TRMCHAIN" );
  hdr->setProperty( "object_type", "Transformation3d" );
  hdr->setProperty( "data_type", "VOID" );
  hdr->setProperty( "transformation_chain", chained );

  dsi.header() = hdr;

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    int i;
    for( i=0, iu=chained.begin(), eu=chained.end(); iu!=eu; ++iu, ++i )
    {
      dsi.list().addDataSource(
        "trm_chain", rc_ptr<DataSource>( new FileDataSource( *iu ) ) );
    }
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

