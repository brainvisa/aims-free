#include <aims/io_soma/trmformatwriter.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/writer.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/utilities/minfutil.h>
#include <soma-io/datasource/filedatasource.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMFORMATWRITER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

bool TrmFormatWriter::filterProperties( Object /* properties */,
                                        Object /* options */ )
{
  // Nothing to filter here
  return true;
}

bool TrmFormatWriter::write( const AffineTransformation3d & obj,
                             rc_ptr<DataSourceInfo> dsi,
                             Object options )
{
  DataSourceList & dsl = dsi->list();
  rc_ptr<DataSource> ds = dsl.dataSource();

  std::string minfname = ds->url() + ".minf";

  dsl.addDataSource( "trm", ds );
  dsl.addDataSource( "minf", carto::rc_ptr<DataSource>
                               ( new FileDataSource( minfname ) ) );

  localMsg( "write " + ds->url() );

  ofstream ddeplacements( ds->url().c_str(), std::ios::out );
  ddeplacements << obj.translation()[0]  << " "
                << obj.translation()[1]  << " "
                << obj.translation()[2]  << endl;
  ddeplacements << obj.rotation()(0,0) << " "
                << obj.rotation()(0,1) << " "
                << obj.rotation()(0,2) << endl;
  ddeplacements << obj.rotation()(1,0) << " "
                << obj.rotation()(1,1) << " "
                << obj.rotation()(1,2) << endl;
  ddeplacements << obj.rotation()(2,0) << " "
                << obj.rotation()(2,1) << " "
                << obj.rotation()(2,2) << endl;
  ddeplacements.close();

  if( obj.header() )
  {
    carto::Object minf = dsi->header();
    if( !minf )
      minf = Object::value( PropertySet() );
    minf->copyProperties( obj.header() );
    if( minf->hasProperty( "file_type" ) )
      minf->removeProperty( "file_type" );
    minf->setProperty( "format", std::string( "TRM" ) );
    minf->setProperty( "data_type", "VOID" );
    minf->setProperty( "object_type",
                       std::string( "AffineTransformation3d" ) );

    // Filter minf to remove irrelevant properties
    // and update some property (uuid) from existing minf
    soma::MinfUtil::filter(minf, options);
    soma::MinfUtil::updateFromSource(
        dsi->list().dataSource( "minf" ).get(),
        minf,
        options
    );

    Writer<carto::GenericObject> minfw(
      dsi->list().dataSource( "minf" )->url() );
    minfw.write( *minf );
  }

  return true;
}


bool Trm3DFormatWriter::filterProperties( Object /* properties */,
                                          Object /* options */ )
{
  // Nothing to filter here
  return true;
}

bool Trm3DFormatWriter::write( const Transformation3d & obj,
                               rc_ptr<DataSourceInfo> dsi,
                               Object options )
{
  const AffineTransformation3d *affobj
    = dynamic_cast<const AffineTransformation3d *>( &obj );
  if( !affobj )
    throw wrong_format_error( "Not an affine transformation" );

  Writer<AffineTransformation3d> writer( dsi->url() );
  writer.setOptions( options );
  return writer.write( *affobj );
}



namespace
{

  bool inittrmformat()
  {
    TrmFormatWriter	*r = new TrmFormatWriter;
    vector<string>	exts;
    exts.push_back( "trm" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "TRM", r, exts );

    Trm3DFormatWriter *r2 = new Trm3DFormatWriter;
    FormatDictionary<Transformation3d>::registerFormat( "AFFINETRANS", r2,
                                                        exts );

    return true;
  }

  bool dummy __attribute__((unused)) = inittrmformat();

}


#undef localMsg
