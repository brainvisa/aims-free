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
#include <aims/io_soma/trmingraphformatreader.h>
#include <aims/io/reader.h>
#include <aims/transformation/transformationgraph3d.h>
#include <aims/data/pheader.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <cartobase/stream/fileutil.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMINGRAPHFORMATREADER" )
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

  bool inittrmingraphformat()
  {
    TrmInGraphFormatReader  *r = new TrmInGraphFormatReader;
    vector<string>  exts;
    exts.push_back( "yaml" );
    FormatDictionary<Transformation3d>::registerFormat( "TRMINGRAPH", r,
                                                        exts );
    return true;
  }

  bool dummy __attribute__((unused)) = inittrmingraphformat();

}


//============================================================================
//   N E W   M E T H O D S
//============================================================================

#include <aims/transformation/transformation_chain.h>
#include <cartobase/smart/rcptrtrick.h>

Transformation3d*
TrmInGraphFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                       const AllocatorContext & context,
                                       Object options )
{
  // cout << "TrmInGraphFormatReader::createAndRead\n";
  rc_ptr<DataSource> ds = dsi->list().dataSource();
  localMsg( "createAndRead " + ds->url() );

  Object hdr = dsi->header();
  if( !hdr->hasProperty( "source_referential" )
      || !hdr->hasProperty( "destination_referential" ) )
    throw runtime_error( "source and/or destination not specified" );

  rc_ptr<TransformationGraph3d> graph;
  Reader<TransformationGraph3d> reader( ds->url() );
  graph.reset( reader.read() );

  string source = hdr->getProperty( "source_referential" )->getString();
  string dest = hdr->getProperty( "destination_referential" )->getString();

  graph->registerInverseTransformations( true );

  Edge *edge = graph->getTransformation( source, dest, true );
  if( !edge )
    throw runtime_error( "transformation not found in graph" );

  Transformation3d *tg = 0;
  {
    rc_ptr<Transformation3d> tr = graph->transformation( edge );

    if( !tr )
      throw runtime_error( "transformation not found in graph" );

    // must release the graph before releasing tr, because the graph might be
    // deleted and downref + delete the transformation inside
    graph.reset( 0 );
    // now we can release tr, and actually prevent its deletion if it's not
    // ref-counted any longer.
    tg = tr.get();
    tr.release();
  } // tr is deleted now
  return tg;
}


// void TrmInGraphFormatReader::read( Transformation3d & obj,
//                                  rc_ptr<DataSourceInfo> dsi,
//                                  const AllocatorContext & /* context */,
//                                  Object options )
// {
//   cout << "TrmInGraphFormatReader::read\n";
// }


FormatReader<Transformation3d>* TrmInGraphFormatReader::clone() const
{
  return new TrmInGraphFormatReader;
}

#undef localMsg


