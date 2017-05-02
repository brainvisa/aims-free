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

#ifndef AIMS_FIBERS_BUNDLESSLICER_H
#define AIMS_FIBERS_BUNDLESSLICER_H


#include <aims/fibers/bundles.h>


namespace aims
{

class BundlesSlicer;


  //-------------------------//
 //  BundlesSlicerListener  //
//-------------------------//

class BundlesSlicerListener
{
public:

  virtual ~BundlesSlicerListener();
  virtual void startBundleSlicing( BundlesSlicer &slicer,
                                   const aims::BundleInfo &bundle );
  virtual void newBundleSlice( BundlesSlicer &slicer,
                               const aims::BundleInfo &bundle, float time,
                               const std::vector< aims::FiberPoint > &points );
  virtual void terminateBundleSlicing( BundlesSlicer &slicer,
                                       const aims::BundleInfo &bundle );
  virtual void noMoreBundleSlice( BundlesSlicer &slicer );
};


  //-----------------//
 //  BundlesSlicer  //
//-----------------//

class BundlesSlicer : public aims::BundleListener
{
public:

  virtual ~BundlesSlicer();
  inline void addBundlesSlicerListener( BundlesSlicerListener & );


protected:

  virtual void startBundleSlicing( const aims::BundleInfo &bundle );
  void produceBundleSlice( const aims::BundleInfo &bundle,
                           float time,
                           const std::vector< aims::FiberPoint > &points );
  virtual void terminateBundleSlicing( const aims::BundleInfo &bundle );
  void stopBundleSliceProduction();
  
private:

  typedef std::vector< BundlesSlicerListener * > BundlesSlicerListenerList;
  BundlesSlicerListenerList _bundlesSlicerListeners;
};



  //-----------------------//
 //  SimpleBundlesSlicer  //
//-----------------------//

class SimpleBundlesSlicer : public BundlesSlicer
{
public:
  SimpleBundlesSlicer( size_t numberOfSlice );

protected:
  void bundleStarted( const aims::BundleProducer &, const aims::BundleInfo & );
  void bundleTerminated( const aims::BundleProducer &,
                         const aims::BundleInfo & );
  void fiberStarted( const aims::BundleProducer &, const aims::BundleInfo &,
                     const aims::FiberInfo & );
  void fiberTerminated( const aims::BundleProducer &, const aims::BundleInfo &,
                        const aims::FiberInfo & );
  void newFiberPoint( const aims::BundleProducer &, const aims::BundleInfo &,
                      const aims::FiberInfo &, const aims::FiberPoint & );
  void noMoreBundle( const aims::BundleProducer & );

private:
  size_t _numberOfSlice;
  std::vector< std::vector< aims::FiberPoint > > _points;
  aims::Polyline _currentCurve;
};


  //----------------------//
 //  BundleSlicesToMesh  //
//----------------------//

class BundleSlicesToMesh : public BundlesSlicerListener
{
public:
  typedef  AimsTimeSurface<2,Void> TimeMesh_t;
  typedef  AimsSurface<2,Void> Mesh_t;
  typedef  AimsVector<uint32_t,2> Polygon_t;

  inline const TimeMesh_t &getMesh() const { return _timeMesh; }

protected:
  void startBundleSlicing( BundlesSlicer &slicer,
                           const aims::BundleInfo &bundle );
  void newBundleSlice( BundlesSlicer &slicer,
                       const aims::BundleInfo &bundle, float time,
                       const std::vector< aims::FiberPoint > &points );
  void terminateBundleSlicing( BundlesSlicer &slicer,
                               const aims::BundleInfo &bundle );
  void noMoreBundleSlice( BundlesSlicer &slicer );

private:
  TimeMesh_t _timeMesh;
};




  //-------------------------//
 //  BundlesSlicerListener  //
//-------------------------//

//-----------------------------------------------------------------------------
inline BundlesSlicerListener::~BundlesSlicerListener() {}


  //-----------------//
 //  BundlesSlicer  //
//-----------------//


//-----------------------------------------------------------------------------
inline void BundlesSlicer::addBundlesSlicerListener( BundlesSlicerListener &
						     listener )
{
  _bundlesSlicerListeners.push_back( &listener );
}



} // namespace aims

#endif // ifndef AIMS_FIBERS_BUNDLESSLICER_H
