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

#include <aims/io_soma/fssurfformatchecker.h>
#include <aims/mesh/surface.h>
#include <aims/io/finder.h>
#include <aims/io/byteswap.h>
#include <aims/resampling/standardreferentials.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/reader/itemreader.h>


using namespace aims;
using namespace soma;
using namespace std;

// from http://www.grahamwideman.com/gw/brain/fs/surfacefileformats.htm

DataSourceInfo FsSurfFormatChecker::check( DataSourceInfo dsi,
                                        DataSourceInfoLoader &,
                                        carto::Object options )
                                        const
{
  // cout << "FsSurfFormatChecker::check\n";

  bool dolist = dsi.list().typecount() == 1;
  if( dolist )
  {
    DataSource* pds = dsi.list().dataSource().get();

    dsi.list().addDataSource( "surf", rc_ptr<DataSource>( pds ) );

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

    rc_ptr<DataSource> ds = dsi.list().dataSource( "surf" );
    if( !ds->isOpen() )
      ds->open( DataSource::Read );
    if( !ds->isOpen() )
      throw file_not_found_error( ds->url() );

    int c = ds->getch();
    ds->ungetch( c );
    if( ds->eof() )
      throw file_not_found_error( ds->url() );

    Object hdr = Object::value( PropertySet() );  // header
    bool res = false;
    if( c == '#' )
      res = checkAsciiSurface( ds, hdr );
    else
      res = checkBinarySurface( ds, hdr );

    if( !res )
      throw wrong_format_error( ds->url() );

    hdr->setProperty( "format", "FSSURF" );
    hdr->setProperty( "object_type", "Mesh" );
    hdr->setProperty( "data_type", "VOID" );

    dsi.header() = hdr;
  }

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    dsi.capabilities().setDataSource( dsi.list().dataSource( "surf" ) );
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


bool FsSurfFormatChecker::checkAsciiSurface( rc_ptr<DataSource> ds,
                                             Object hdr ) const
{
  string line;
  bool r;
  r = StreamUtil::getline( *ds, line );
  if( !r || ds->eof() || line.substr( 0, 19 ) != "#!ascii version of " )
    return false;
  r = StreamUtil::getline( *ds, line );
  if( !r || ds->eof() )
    return false;
  stringstream ss( line );
  int nv, np;
  ss >> nv;
  ss >> np;
  if( nv <= 0 || np <= 0 )
    return false;
  hdr->setProperty( "ascii", 1 );
  hdr->setProperty( "vertex_number", nv );
  hdr->setProperty( "polygon_dimension", np );
  return true;
}


bool FsSurfFormatChecker::checkBinarySurface( rc_ptr<DataSource> ds,
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
  if( magic == 0xfffffe )
  {
    hdr->setProperty( "polygon_dimension", 3 );
    itemr.reset( dir.reader( true, bswap ) );
    string comment = StreamUtil::readUntil( *ds, "\n\0" );
    if( !ds->isOpen() || ds->eof() )
      return false;
    int c = ds->getch();  // skip final "\n"
    c = ds->getch();      // it is generally doubled.
    if( !ds->isOpen() || ds->eof() )
      return false;
    if( c != 0x0a )
      ds->ungetch( c );
    hdr->setProperty( "comment", comment );
    vector<int32_t> nv( 2, 0 );
    if( itemr->read( *ds, &nv[0], 2 ) != 2 || !ds->isOpen()
        || ds->eof() )
      return false;
    hdr->setProperty( "vertex_number", nv[0] );
    hdr->setProperty( "polygon_number", nv[1] );
  }
  else if( magic == 0xffffff )
  {
    hdr->setProperty( "polygon_dimension", 4 );
    // itemr.reset( dir.reader( true, bswap ) );
  }
  else
  {
    // cout << "not a FS surf file: magic: " << magic << endl;
    return false;
  }
  hdr->setProperty( "byte_swapping", int(bswap) );
  // meshes are in RAS orientation
  Object mat = Object::value( PropertySet() );
  mat->setProperty( "front_face", "counterclockwise" );
  hdr->setProperty( "material", mat );
  // the mesh referential is not MNI/ICBM but is RAS oriented.
  Object ref_def = Object::value( Dictionary() );
  ref_def->setProperty( "axes_orientation", "RAS" );
  hdr->setProperty( "referential_def", ref_def );

  return true;
}


