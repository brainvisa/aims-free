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


