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

#include <aims/io_soma/trmgraphformatwriter.h>
#include <aims/transformation/transformationgraph3d.h>
#include <soma-io/transformation/transformation.h>
#include <aims/io/writer.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <cartobase/stream/fileutil.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMGRAPHFORMATWRITER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

bool TrmGraphFormatWriter::filterProperties( Object /* properties */,
                                             Object /* options */ )
{
  // Nothing to filter here
  return true;
}

bool TrmGraphFormatWriter::write( const TransformationGraph3d & obj,
                                  rc_ptr<DataSourceInfo> dsi,
                                  Object options )
{
  rc_ptr<DataSource> ds = dsi->list().dataSource();

  localMsg( "write " + ds->url() );

  bool allow_read = true;
  bool affine_only = false;
  bool release_loaded = false;
  bool embed_affines = false;

  if( options && options->hasProperty( "allow_read" ) )
    allow_read = bool( options->getProperty( "allow_read" )->getScalar() );
  if( options && options->hasProperty( "affine_only" ) )
    affine_only = bool( options->getProperty( "affine_only" )->getScalar() );
  if( options && options->hasProperty( "release_loaded" ) )
    release_loaded
      = bool( options->getProperty( "release_loaded" )->getScalar() );
  if( options && options->hasProperty( "embed_affines" ) )
    embed_affines
      = bool( options->getProperty( "embed_affines" )->getScalar() );

  list<Edge *> loaded;
  if( release_loaded && allow_read )
    loaded = const_cast<TransformationGraph3d &>(
      obj ).loadAffineTransformations();

  Object gobj = obj.asDict( affine_only, allow_read, embed_affines );
  Writer<Object> w( ds->url() );
  w.write( gobj );

  // write each individual transform
  string dirname = FileUtil::dirname( dsi->url() );

  Object sit = gobj->objectIterator();
  for( ; sit->isValid(); sit->next() )
  {
    string sid = sit->key();
    Object dit = sit->currentValue()->objectIterator();
    for( ; dit->isValid(); dit->next() )
    {
      string did = dit->key();
      if( dit->currentValue()->type() == DataTypeCode<vector<float> >::name() )
        continue;  // embedded affine matrix: don't save it

      Edge *etr = obj.getTransformation_raw( sid, did );
      if( etr )
      {
        bool release = false;
        rc_ptr<Transformation3d> tr( obj.transformation( etr ) );
        if( !tr && allow_read )
        {
          tr = obj.loadTransformation( etr, affine_only );
          if( release_loaded )
            release = true;
        }
        if( tr )
        {
          string filename = dit->currentValue()->getString();
          filename = dirname + FileUtil::separator() + filename;
          Writer<Transformation3d> w( filename );
          w.write( *tr );
          if( release )
            etr->setProperty( "transformation", rc_ptr<Transformation3d>() );
        }
      }
    }
  }

  if( release_loaded )
  {
    list<Edge *>::iterator il, el = loaded.end();
    for( il=loaded.begin(); il!=el; ++il )
      (*il)->setProperty( "transformation", rc_ptr<Transformation3d>() );
  }

  return true;
}


namespace
{

  bool inittrmgformat()
  {
    TrmGraphFormatWriter	*w = new TrmGraphFormatWriter;
    vector<string>	exts;
    exts.push_back( "yaml" );
    exts.push_back( "trmg" );
    FormatDictionary<TransformationGraph3d>::registerFormat( "TRMGRAPH", w,
                                                             exts );
    return true;
  }

  bool dummy __attribute__((unused)) = inittrmgformat();

}


#undef localMsg
