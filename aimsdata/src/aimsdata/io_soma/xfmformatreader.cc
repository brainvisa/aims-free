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
#include <aims/io_soma/xfmformatreader.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/data/pheader.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/io/reader.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <cartobase/stream/fileutil.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "XFMFORMATREADER" )
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

  bool initxfmformat()
  {
    XfmFormatReader  *r = new XfmFormatReader;
    vector<string>  exts;
    exts.push_back( "xfm" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "XFM", r, exts );

    XfmT3DFormatReader *r2 = new XfmT3DFormatReader;
    exts.push_back( "xfm" );
    FormatDictionary<Transformation3d>::registerFormat( "XFM3D", r2,
                                                        exts );

    return true;
  }

  bool dummy __attribute__((unused)) = initxfmformat();

}


//============================================================================
//   N E W   M E T H O D S
//============================================================================

AffineTransformation3d*
XfmFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
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


void XfmFormatReader::read( AffineTransformation3d & obj,
                            rc_ptr<DataSourceInfo> dsi,
                            const AllocatorContext & /* context */,
                            Object options )
{
  // cout << "XfmFormatReader::read " << dsi->list().dataSource()->url() << endl;
  rc_ptr<DataSource> ds = dsi->list().dataSource();

  if( !ds->isOpen() )
    ds->open( DataSource::Read );
  if( !ds->isOpen() )
    throw file_not_found_error( ds->url() );

  AsciiDataSourceTraits<float>		fir;
  string s;
  bool r;
  bool mni = false;
  do
  {
    r = StreamUtil::getline( *ds, s );
    if( r && s.substr( 0, 18 ) == "MNI Transform File" )
      mni = true;
  }
  while( r && !ds->eof() && s.substr( 0, 18 ) != "Linear_Transform =" );
  if( !r || ds->eof() )
    throw wrong_format_error( ds->url() );

  unsigned	i, j;
  for( i=0; i<3; ++i )
    for( j=0; j<4; ++j )
      {
        if( !StreamUtil::skip( *ds, " \t\n\r" ) )
          throw wrong_format_error( ds->url() );
        if( !fir.read( *ds, obj.matrix()( i, j ) ) )
          throw wrong_format_error( ds->url() );
      }
  ds->close();

  Object  hdr = Object::value( PropertySet() );
  rc_ptr<DataSource> mds( 0 );
  try
  {
    mds = dsi->list().dataSource( "minf" );
    if( mds.get() )
      DataSourceInfoLoader::readMinf( *mds, hdr, options );
  }
  catch( ... )
  {
  }
  if( mni )
    hdr->setProperty(
      "destination_referential",
      StandardReferentials::mniTemplateReferentialID() );

  obj.setHeader( hdr );

  bool inv = false;
  try
  {
    Object oinv = options->getProperty( "inv" );
    inv = bool( oinv->getScalar() );
    if( inv )
      obj = *obj.inverse();
  }
  catch( runtime_error & )
  {
  }

  // this is done after inv because the dsi header has also been inverted
  if( dsi->header().get() )
    obj.header()->copyProperties( dsi->header() );

  // cout << "read XFM OK\n";
}


FormatReader<AffineTransformation3d>* XfmFormatReader::clone() const
{
  return new XfmFormatReader;
}


// ---

Transformation3d*
XfmT3DFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                   const AllocatorContext & context,
                                   Object options )
{
  Reader<AffineTransformation3d> reader( dsi );
  reader.setAllocatorContext( context );
  reader.setOptions( options );
  return reader.read();
}


Transformation3d* XfmT3DFormatReader::create( carto::Object header,
                                              const AllocatorContext & context,
                                              carto::Object options )
{
  return new AffineTransformation3d;
}


void XfmT3DFormatReader::read( Transformation3d & obj,
                               rc_ptr<DataSourceInfo> dsi,
                               const AllocatorContext & context,
                               Object options )
{
  // cout << "XfmT3DFormatReader::read\n";
  AffineTransformation3d *affobj
    = dynamic_cast<AffineTransformation3d *>( &obj );
  if( !affobj )
    throw wrong_format_error( "Not an affine transformation" );

  Reader<AffineTransformation3d> reader( dsi );
  reader.setAllocatorContext( context );
  reader.setOptions( options );
  reader.read( *affobj );
  // cout << "read XFM OK\n";
}


FormatReader<Transformation3d>* XfmT3DFormatReader::clone() const
{
  return new XfmT3DFormatReader;
}


#undef localMsg

