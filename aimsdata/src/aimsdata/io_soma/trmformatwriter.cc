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

#include <aims/io_soma/trmformatwriter.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/motionW.h>
#include <soma-io/io/formatdictionary.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMFORMATWRITER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

bool TrmFormatWriter::filterProperties( Object /* properties */,
                                        Object /* options */ )
{
  // Nothing to filter here
  return true;
}

bool TrmFormatWriter::write( const AffineTransformation3d & obj,
                             rc_ptr<DataSourceInfo> dsi,
                             Object /*options*/ )
{
  rc_ptr<DataSource> ds = dsi->list().dataSource();

  localMsg( "write " + ds->url() );

  MotionWriter mw( ds->url() );
  mw.write( obj );

  return true;
}


namespace
{

  bool inittrmformat()
  {
    TrmFormatWriter	*r = new TrmFormatWriter;
    vector<string>	exts;
    exts.push_back( "trm" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "TRM", r, exts );
    return true;
  }

  bool dummy __attribute__((unused)) = inittrmformat();

}


#undef localMsg
