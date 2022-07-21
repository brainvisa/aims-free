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

