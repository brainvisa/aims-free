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

