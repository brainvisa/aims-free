#include <aims/io_soma/trmgraphformatchecker.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/finder.h>
#include <aims/data/pheader.h>
#include <cartobase/stream/fileutil.h>


using namespace aims;
using namespace soma;
using namespace std;

DataSourceInfo TrmGraphFormatChecker::check( DataSourceInfo dsi,
                                             DataSourceInfoLoader &,
                                             carto::Object options )
                                             const
{
  // cout << "TrmGraphFormatChecker::check\n";

  string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );

  // 1. check the file as a TransformationGraph3d

  // TODO FIXME
  if( url.length() < 5 || url.substr( url.length() - 5, 5 ) != ".yaml" )
    throw wrong_format_error( "Not A transform-in-graph",
                              dsi.list().dataSource()->url() );

  // 2. check that source and destinations are specified

  if( options
      && ( options->hasProperty( "source" )
           || options->hasProperty( "source_referential" ) )
      && ( options->hasProperty( "destination" )
           || options->hasProperty( "dest" )
           || options->hasProperty( "destination_referential" ) ) )
    throw wrong_format_error( "Not A transform graph",
                              dsi.list().dataSource()->url() );

  Object hdr = Object::value( PropertySet() );  // header

  hdr->setProperty( "format", "TRMGRAPH" );
  hdr->setProperty( "object_type", "TransformationGraph3d" );
  hdr->setProperty( "data_type", "VOID" );

  dsi.header() = hdr;

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
//     dsi.capabilities().setDataSource( dsi.list().dataSource( "ima" ) );
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

