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

#ifndef AIMS_FIBERS_BUNDLESFEATURES_H
#define AIMS_FIBERS_BUNDLESFEATURES_H


#include <iostream>
#include <cartobase/config/verbose.h>
#include <cartobase/object/property.h>
#include <aims/resampling/linearInterpolator.h>
#include <aims/fibers/bundlesSlicer.h>
#include <aims/features/point_set_features.h>


namespace aims {

  //-------------------//
 //  BundlesFeatures  //
//-------------------//

class BundlesFeatures
  : public aims::BundleListener, public BundlesSlicerListener
{
public:
  BundlesFeatures();

  void addImageStatistics( const std::string &name, 
                           const carto::rc_ptr< aims::Interpolator > & );
  void write( const std::string &fileName ) const;
  void write( std::ostream &out ) const;


private:

  // BundleListener virtual methods
  virtual void bundleStarted( const aims::BundleProducer &,
                              const aims::BundleInfo & );
  virtual void bundleTerminated( const aims::BundleProducer &,
                                 const aims::BundleInfo & );
  virtual void fiberStarted( const aims::BundleProducer &,
                             const aims::BundleInfo &,
                             const aims::FiberInfo & );
  virtual void fiberTerminated( const aims::BundleProducer &,
                                const aims::BundleInfo &,
                                const aims::FiberInfo & );
  virtual void newFiberPoint( const aims::BundleProducer &,
                              const aims::BundleInfo &,
                              const aims::FiberInfo &,
                              const aims::FiberPoint & );
  virtual void noMoreBundle( const aims::BundleProducer & );
  
  // BundleSlicerListener virtual methods
  void startBundleSlicing( BundlesSlicer &slicer,
                           const aims::BundleInfo &bundle );
  void newBundleSlice( BundlesSlicer &slicer,
                       const aims::BundleInfo &bundle, float abscissa,
                       const std::vector< aims::FiberPoint > &points );
  void terminateBundleSlicing( BundlesSlicer &slicer,
                               const aims::BundleInfo &bundle );
  void noMoreBundleSlice( BundlesSlicer &slicer );

  double _currentLength;
  bool _firstPoint;
  aims::FiberPoint _lastPoint;
  int _fiberCount;

  typedef std::map< std::string,
                    carto::rc_ptr< aims::Interpolator > > Interpolators_t;
  typedef std::map< std::string, 
                    aims::ScalarFeaturesProvider::Scalar_t > String_Scalar_t;
  typedef std::map< std::string, 
                    std::vector< aims::ScalarFeaturesProvider::Scalar_t > > 
  String_ScalarSet_t;

  typedef std::map< std::string, std::map< std::string, String_Scalar_t > >
  BundlesFeatures_t;
  typedef std::map< std::string, std::map< std::string, String_ScalarSet_t > >
  BundlesSliceFeatures_t;


  aims::ScalarSetFeatures _scalarSetFeatures;
  Interpolators_t _interpolators;

  carto::Object _result;

  String_ScalarSet_t _currentBundleFeaturesValues;
};






} // namespace aims

#endif // ifndef AIMS_FIBERS_BUNDLESFEATURES_H
