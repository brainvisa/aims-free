/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/io/graphBundlesFormat.h>
#include <aims/fibers/bundles.h>

using namespace std;
using namespace carto;
using namespace aims;
using namespace comist;

GraphBundlesFormat::~GraphBundlesFormat()
{
}


namespace
{

  void setBundlesOptions( BundleToGraph & bundleToGraph, carto::Object options )
  {
    try
    {
      Object owid = options->getProperty( "fiber_width" );
      float width = owid->getScalar();
      bundleToGraph.setProperty( "width", width );
    }
    catch( ... )
    {
    }
    try
    {
      Object onp = options->getProperty( "num_points_in_mesh_section" );
      int np = int( round( onp->getScalar() ) );
      bundleToGraph.setProperty( "num_points_in_mesh_section", np );
    }
    catch( ... )
    {
    }
    try
    {
      Object osp = options->getProperty( "normals_as_fiber_start_pos" );
      bool sp = bool( round( osp->getScalar() ) );
      bundleToGraph.setProperty( "normals_as_fiber_start_pos", sp );
    }
    catch( ... )
    {
    }
    try
    {
      Object osp = options->getProperty( "relative_fiber_start_pos" );
      Point3df pos( 0, 0, 0 );
      int i;
      Object it = osp->objectIterator();
      while( i<3 && it->isValid() )
      {
        pos[i] = float( it->currentValue()->getScalar() );
        it->next();
        ++i;
      }
      bundleToGraph.setProperty( "relative_fiber_start_pos", pos );
    }
    catch( ... )
    {
    }
    try
    {
      Object osp = options->getProperty( "auto_fiber_start_pos" );
      bool sp = bool( round( osp->getScalar() ) );
      bundleToGraph.setProperty( "auto_fiber_start_pos", sp );
    }
    catch( ... )
    {
    }
  }

}


bool GraphBundlesFormat::read( const string & filename, Graph & obj, 
                               const carto::AllocatorContext & /*context*/, 
                               carto::Object options )
{
  obj.clear();
  obj.clearProperties();
  BundleReader reader( filename );
  Object header = reader.readHeader();
  BundleToGraph bundleToGraph( obj );
  setBundlesOptions( bundleToGraph, options );
  reader.addBundleListener( bundleToGraph );
  obj.copyProperties( header );
  reader.read();
  return true;
}


Graph* GraphBundlesFormat::read( const string & filename, 
                                 const carto::AllocatorContext & /*context*/, 
                                 carto::Object options )
{
  Graph *result = new Graph( "RoiArg" );
  BundleReader reader( filename );
  Object header = reader.readHeader();
  BundleToGraph bundleToGraph( *result );
  setBundlesOptions( bundleToGraph, options );
  reader.addBundleListener( bundleToGraph );
  result->copyProperties( header );
  reader.read();
  return result;
}
