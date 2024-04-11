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

#include <aims/io_soma/trmformatchecker.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/finder.h>
#include <aims/data/pheader.h>
#include <aims/io_soma/trmformatreader.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>


using namespace aims;
using namespace soma;
using namespace std;

DataSourceInfo TrmFormatChecker::check( DataSourceInfo dsi,
                                        DataSourceInfoLoader &,
                                        carto::Object options )
                                        const
{
  // cout << "TrmFormatChecker::check " << dsi.list().dataSource()->url() << endl;;

  bool dolist = dsi.list().typecount() == 1;
  if( dolist )
  {
    DataSource* pds = dsi.list().dataSource().get();

    dsi.list().addDataSource( "trm", rc_ptr<DataSource>( pds ) );

    string url = FileUtil::uriFilename( pds->url() );
    string minfname = url + ".minf";

    //// Minf DataSource
    if( FileUtil::fileStat( minfname ).find( '+' ) != string::npos )
    {
      // if minf file exist
      dsi.list().addDataSource(
        "minf", rc_ptr<DataSource>( new FileDataSource( minfname ) ) );
    }
    else
    {
      // we suppose original url is minf
      dsi.list().addDataSource( "minf", rc_ptr<DataSource>( pds ) );
    }
  }

  bool doread = dsi.header().isNone();
  if( doread )
  {
    string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );
    // cout << "URL: " << url << endl;

    // simple test: try to read all...
    TrmFormatReader mr;
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

    hdr->copyProperties( Object::reference( *m.header() ) );

    hdr->setProperty( "format", "TRM" );
    hdr->setProperty( "object_type", "AffineTransformation3d" );
    hdr->setProperty( "data_type", "VOID" );

    dsi.header() = hdr;

    // add meta-info to header
    DataSource* minfds = dsi.list().dataSource( "minf" ).get();
    DataSourceInfoLoader::readMinf( *minfds, dsi.header(), options );

    try
    {
      Object oinv = options->getProperty( "inv" );
      bool inv = bool( oinv->getScalar() );
      if( inv )
      {
        Object s;
        if( hdr->hasProperty( "source_referential" ) )
        {
          s = hdr->getProperty( "source_referential" );
          hdr->removeProperty( "source_referential" );
        }
        if( hdr->hasProperty( "destination_referential" ) )
        {
          hdr->setProperty( "source_referential",
                            hdr->getProperty( "destination_referential" ) );
          hdr->removeProperty( "destination_referential" );
        }
        if( s )
          hdr->setProperty( "destination_referential", s );
      }
    }
    catch( runtime_error & )
    {
    }

  }

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
    dsi.capabilities().setDataSource( dsi.list().dataSource( "trm" ) );
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

