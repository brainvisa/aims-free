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

//-------------------------------------------------------------------
#include <aims/io_soma/trmformatreader.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/data/pheader.h>
#include <aims/io/motionR.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/io/reader.h>
#include <cartobase/stream/fileutil.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMFORMATREADER" )
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

  bool inittrmformat()
  {
    TrmFormatReader  *r = new TrmFormatReader;
    vector<string>  exts;
    exts.push_back( "trm" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "TRM", r, exts );

    TrmT3DFormatReader *r2 = new TrmT3DFormatReader;
    exts.push_back( "trmhdr" );
    exts.push_back( "trmc" );
    FormatDictionary<Transformation3d>::registerFormat( "AFFINETRANS", r2,
                                                        exts );

    return true;
  }

  bool dummy __attribute__((unused)) = inittrmformat();

}


//============================================================================
//   N E W   M E T H O D S
//============================================================================

AffineTransformation3d*
TrmFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                const AllocatorContext & context,
                                Object options )
{
  rc_ptr<DataSource> ds = dsi->list().dataSource();
  localMsg( "createAndRead " + ds->url() );
  AffineTransformation3d *tr = new AffineTransformation3d;
  try
  {
    read( *tr, dsi, context, options );
  }
  catch( ... )
  {
    delete tr;
    throw;
  }
  tr->header()->copyProperties( dsi->header() );
  return tr;
}


void TrmFormatReader::read( AffineTransformation3d & obj,
                            rc_ptr<DataSourceInfo> dsi,
                            const AllocatorContext & /* context */,
                            Object options )
{
  // cout << "TrmFormatReader::read\n";
  rc_ptr<DataSource> ds = dsi->list().dataSource();
  MotionReader r( FileUtil::uriFilename( ds->url() ) );
  r.read( obj );
  obj.header()->copyProperties( dsi->header() );
  try
  {
    Object oinv = options->getProperty( "inv" );
    bool inv = bool( oinv->getScalar() );
    if( inv )
    {
      obj = obj.inverse();
    }
  }
  catch( runtime_error & )
  {
  }
  // cout << "read TRM OK\n";
}


FormatReader<AffineTransformation3d>* TrmFormatReader::clone() const
{
  return new TrmFormatReader;
}


// ---

template <>
Transformation3d* FormatReader<Transformation3d>::create(
  carto::Object header, const AllocatorContext & context,
  carto::Object options )
{
  return new AffineTransformation3d;
}


Transformation3d*
TrmT3DFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                   const AllocatorContext & context,
                                   Object options )
{
  Reader<AffineTransformation3d> reader( dsi );
  reader.setAllocatorContext( context );
  reader.setOptions( options );
  return reader.read();
}


Transformation3d* TrmT3DFormatReader::create( carto::Object header,
                                              const AllocatorContext & context,
                                              carto::Object options )
{
  return new AffineTransformation3d;
}


void TrmT3DFormatReader::read( Transformation3d & obj,
                               rc_ptr<DataSourceInfo> dsi,
                               const AllocatorContext & context,
                               Object options )
{
  // cout << "TrmT3DFormatReader::read\n";
  AffineTransformation3d *affobj
    = dynamic_cast<AffineTransformation3d *>( &obj );
  if( !affobj )
    throw wrong_format_error( "Not an affine transformation" );

  Reader<AffineTransformation3d> reader( dsi );
  reader.setAllocatorContext( context );
  reader.setOptions( options );
  reader.read( *affobj );
  // cout << "read TRM OK\n";
}


FormatReader<Transformation3d>* TrmT3DFormatReader::clone() const
{
  return new TrmT3DFormatReader;
}


#undef localMsg

// instantiate FormatReader<AffineTransformation3d>
#include <soma-io/reader/formatreader_d.h>

template class FormatReader<AffineTransformation3d>;
template class FormatReader<Transformation3d>;

