/* Copyright (c) 1995-2017 CEA
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

#include <aims/fibers/bundlesSlicer.h>


using namespace std;

namespace aims {


  //-------------------------//
 //  BundlesSlicerListener  //
//-------------------------//

//-----------------------------------------------------------------------------
BundlesSlicer::~BundlesSlicer() {}

//-----------------------------------------------------------------------------
void BundlesSlicerListener::startBundleSlicing( BundlesSlicer &,
                         const BundleInfo & )
{}


//-----------------------------------------------------------------------------
void BundlesSlicerListener::newBundleSlice( BundlesSlicer &,
                     const BundleInfo &, float ,
                     const std::vector< FiberPoint > & )
{}


//-----------------------------------------------------------------------------
 void BundlesSlicerListener::terminateBundleSlicing( BundlesSlicer &,
				       const BundleInfo & )
{}


//-----------------------------------------------------------------------------
void BundlesSlicerListener::noMoreBundleSlice( BundlesSlicer & )
{}


  //-----------------//
 //  BundlesSlicer  //
//-----------------//

//-----------------------------------------------------------------------------
void BundlesSlicer::produceBundleSlice( const BundleInfo &bundle,
                                        float time,
                                        const vector< FiberPoint > &points )
{
  for( BundlesSlicerListenerList::iterator i = _bundlesSlicerListeners.begin();
       i != _bundlesSlicerListeners.end(); ++i ) {
    (*i)->newBundleSlice( *this, bundle, time, points );
  }
}


//-----------------------------------------------------------------------------
void BundlesSlicer::startBundleSlicing( const BundleInfo &bundle )
{
  for( BundlesSlicerListenerList::iterator i = _bundlesSlicerListeners.begin();
       i != _bundlesSlicerListeners.end(); ++i ) {
    (*i)->startBundleSlicing( *this, bundle );
  }
}


//-----------------------------------------------------------------------------
void BundlesSlicer::terminateBundleSlicing( const BundleInfo &bundle )
{
  for( BundlesSlicerListenerList::iterator i = _bundlesSlicerListeners.begin();
       i != _bundlesSlicerListeners.end(); ++i ) {
    (*i)->terminateBundleSlicing( *this, bundle );
  }
}


//-----------------------------------------------------------------------------
void BundlesSlicer::stopBundleSliceProduction()
{
  for( BundlesSlicerListenerList::iterator i = _bundlesSlicerListeners.begin();
       i != _bundlesSlicerListeners.end(); ++i ) {
    (*i)->noMoreBundleSlice( *this );
  }
}


  //-----------------------//
 //  SimpleBundlesSlicer  //
//-----------------------//

SimpleBundlesSlicer::SimpleBundlesSlicer( size_t numberOfSlice ):
  _numberOfSlice( numberOfSlice )
{
  _points.reserve( numberOfSlice );
}


//-----------------------------------------------------------------------------
void SimpleBundlesSlicer::bundleStarted( const BundleProducer &,
                                         const BundleInfo &bundle )
{
  _points.clear();
  _points.resize( _numberOfSlice );
  startBundleSlicing( bundle );
}


//-----------------------------------------------------------------------------
void SimpleBundlesSlicer::bundleTerminated( const BundleProducer &,
                                            const BundleInfo &bundle )
{
  for( size_t i = 0; i < _numberOfSlice; ++i ) {
    produceBundleSlice( bundle, i, _points[ i ] );
  }
  terminateBundleSlicing( bundle );
}


//-----------------------------------------------------------------------------
void SimpleBundlesSlicer::fiberStarted( const BundleProducer &,
                                        const BundleInfo &,
                                        const FiberInfo & )
{
  _currentCurve.clear();
}


//-----------------------------------------------------------------------------
void SimpleBundlesSlicer::fiberTerminated( const BundleProducer &,
                                           const BundleInfo &,
                                           const FiberInfo & )
{
  vector< FiberPoint > result;
  _currentCurve.resample( _numberOfSlice, result );
  if ( result.size() == _numberOfSlice ) {
    for( size_t i = 0; i < _numberOfSlice; ++i ) {
      _points[ i ].push_back( result[ i ] );
    }
  }
}


//-----------------------------------------------------------------------------
void SimpleBundlesSlicer::newFiberPoint( const BundleProducer &,
                                         const BundleInfo &,
                                         const FiberInfo &,
                                         const FiberPoint &point )
{
  _currentCurve.push_back( point );
}


//-----------------------------------------------------------------------------
void SimpleBundlesSlicer::noMoreBundle( const BundleProducer & )
{
  stopBundleSliceProduction();
}


  //----------------------//
 //  BundleSlicesToMesh  //
//----------------------//

//-----------------------------------------------------------------------------
void BundleSlicesToMesh::startBundleSlicing( BundlesSlicer &,
					     const BundleInfo & )
{
}

//-----------------------------------------------------------------------------
void  BundleSlicesToMesh::
newBundleSlice( BundlesSlicer &,
		const BundleInfo &, 
		float time,
		const std::vector< FiberPoint > &points )
{
  Mesh_t &mesh = _timeMesh[ int( time * 10000 ) ];
  // Mesh_t &mesh = _timeMesh[ 0 ];
  size_t first = mesh.vertex().size();
  for( vector< FiberPoint >::const_iterator it = points.begin();
       it != points.end(); ++it ) {
    mesh.vertex().push_back( *it );
  }
  // for( size_t i = first; i <  mesh.vertex().size(); ++i ) {
  for( size_t j = first + 1; j <  mesh.vertex().size(); ++j ) {
    if ( mesh.vertex()[ first ] != mesh.vertex()[ j ] ) {
      mesh.polygon().push_back( Polygon_t( first, j ) );
    }
    //    }
  }
}


//-----------------------------------------------------------------------------
void  BundleSlicesToMesh::terminateBundleSlicing( BundlesSlicer &,
						  const BundleInfo & )
{
}

//-----------------------------------------------------------------------------
void  BundleSlicesToMesh::noMoreBundleSlice( BundlesSlicer & )
{
}


} // namespace aims

