//-------------------------------------------------------------------
#include <aims/io_soma/trm_header_formatreader.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/data/pheader.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasource/datasource.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMHEADERFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

//============================================================================
//   I N I T
//============================================================================

namespace
{

  bool inittrmhdrformat()
  {
    TrmHeaderFormatReader  *r = new TrmHeaderFormatReader;
    vector<string>  exts;
    exts.push_back( "trmhdr" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "TRMHEADER", r,
                                                              exts );
    return true;
  }

  bool dummy __attribute__((unused)) = inittrmhdrformat();

}


//============================================================================
//   N E W   M E T H O D S
//============================================================================

AffineTransformation3d*
TrmHeaderFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                      const AllocatorContext & /* context */,
                                      Object options )
{
  rc_ptr<DataSource> ds = dsi->list().dataSource();
  localMsg( "createAndRead " + ds->url() );
  AffineTransformation3d *tr
    = new AffineTransformation3d(
      dsi->header()->getProperty( "transformation" ) );
  tr->header()->copyProperties( dsi->header() );
  return tr;
}


void TrmHeaderFormatReader::read( AffineTransformation3d & obj,
                                  rc_ptr<DataSourceInfo> dsi,
                                  const AllocatorContext & /* context */,
                                  Object options )
{
  obj = AffineTransformation3d(
    dsi->header()->getProperty( "transformation" ) );
  obj.header()->copyProperties( dsi->header() );
}


FormatReader<AffineTransformation3d>* TrmHeaderFormatReader::clone() const
{
  return new TrmHeaderFormatReader;
}

#undef localMsg

// instantiate FormatReader<AffineTransformation3d>
#include <soma-io/reader/formatreader_d.h>

namespace carto {
template class FormatReader<AffineTransformation3d>;
}
