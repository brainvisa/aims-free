/* Copyright (c) 1995-2007 CEA
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

 
#include <aims/getopt/getopt2.h>
#include <aims/signalfilter/g3dsmooth.h>
#include <aims/data/data_g.h>
#include <aims/roi/roi_g.h>
#include <aims/roi/roiIterator.h>
#include <aims/roi/maskIterator.h>
#include <aims/vector/vector.h>
#include <aims/roi/roistat.h>
#include <aims/roi/roigtm.h>
#include <aims/io/roiR.h>
#include <aims/io/roigtmR.h>
#include <aims/io/motionR.h>
#include <aims/resampling/resampling_g.h>
#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <aims/io/roiselectorR.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/labelImage.h>
#include <cartobase/stream/directory.h>
#include <cartobase/smart/rcptr.h>
#include <aims/distancemap/chamfer.h>

using namespace aims;
using namespace carto;
using namespace std;

struct PointLess : public std::binary_function< Point3d, Point3d , bool>
{
  bool operator () ( const Point3d & p1, const Point3d & p2 ) const
  {
    return( p1[2] < p2[2] 
	    || ( (p1[2] == p2[2]) && (p1[1] < p2[1])  )
	    || ( (p1[2] == p2[2]) 
		 && (p1[1] == p2[1]) && (p1[0] < p2[0]) ) ) ;
  }
};

inline
bool 
in( const Point3d& dims, const Point3d& p )
{
  if ( p[0] < 0 || p[0] > dims[0] - 1 ||  
       p[1] < 0 || p[1] > dims[1] - 1 ||
       p[2] < 0 || p[2] > dims[2] - 1 )
    return false ;
  
  return true ;
}

Point3d 
neighborhoodSize( const Point3df& voxSize, short minNbPts ) {
  int indMax = 0, indMin1 = 1, indMin2 = 2 ; 
  if( voxSize[1] > voxSize[0] ) { indMax = 1 ; indMin1 = 0 ; indMin2 = 2 ; }
  if( voxSize[2] > voxSize[indMax] ) { indMax = 2 ; indMin1 = 0 ; indMin2 = 1 ; }
  
  Point3d mhs ;
  mhs[indMax] = 1 ;
  mhs[indMin1] = int( voxSize[indMax] / voxSize[indMin1] + 0.5 ) ;
  mhs[indMin2] = int( voxSize[indMax] / voxSize[indMin2] + 0.5 ) ;
  
  while( (2*mhs[0]+1) * (2*mhs[0]+1) * (2*mhs[0]+1) < minNbPts ){
    ++mhs[0] ;
    ++mhs[1] ;
    ++mhs[2] ;      
  }
  
  return mhs ;
}

inline double
recError( const Point3d& p, rc_ptr< AimsData<float> > data, 
	  const AimsData<float>& mean, const vector<float>& normFactor,
	  bool iterativeRecons )
{
  double res = 0., normSum = 0. ;
  
  if( iterativeRecons ){
    for( int t = 0 ; t < data->dimT() ; ++t ){
      if( mean( t ) != 0. ){
	res += sqr( ( (*data)(p[0], p[1], p[2], t) - mean(t) ) * normFactor[t] ) / mean(t) ;
	normSum += sqr(normFactor[t]) / mean(t) ;
      }
    }
  } else {
    for( int t = 0 ; t < data->dimT() ; ++t ){
      res += sqr( ( (*data)(p[0], p[1], p[2], t) - mean(t) ) * normFactor[t] ) ;
      normSum += sqr(normFactor[t] ) ;
    }
  }
  
  return res / normSum * data->dimT() ;
}

inline double
weightedMeanOverTime( const vector<float>& vec, const vector<float>& mean, 
		      const vector<float>& normFactor,
		      bool iterativeRecons )
{
  double res = 0., normSum = 0. ;
  
  if( iterativeRecons ){
    for( unsigned int t = 0 ; t < mean.size() ; ++t ){
      if( mean[t] != 0. ){
	res += vec[t] * sqr(normFactor[t]) / mean[t] ;
	normSum += sqr(normFactor[t]) / mean[t] ;
      }
    }
  } else {
    for( unsigned int t = 0 ; t < mean.size() ; ++t ){
      res += vec[t] * sqr(normFactor[t]) ;
      normSum += sqr(normFactor[t] ) ;

    }
  }
  
  return res / normSum ;
}

inline double
weightedSqrtMeanOverTime( const vector<float>& vec, const vector<float>& mean, 
			  const vector<float>& normFactor,
			  bool iterativeRecons )
{
  double res = 0., normSum = 0. ;
  
  if( iterativeRecons ){
    for( unsigned int t = 0 ; t < mean.size() ; ++t ){
      if( mean[t] != 0. ){
	res += vec[t] * sqr(normFactor[t]) / mean[t] ;
	normSum += sqr(normFactor[t]) / mean[t] ;
      }
    }
  } else {
    for( unsigned int t = 0 ; t < mean.size() ; ++t ){
      res += vec[t] * sqr(normFactor[t]) ;
      normSum += sqr(normFactor[t] ) ;

    }
  }
  
  return res / normSum ;
}

class ActivityWeightStrategy{
public:
  ActivityWeightStrategy() {}
  virtual ~ActivityWeightStrategy() {}
  
  virtual float weight( Point3d& p ) = 0 ;
  
private:
  
};

class LocalSpatialVarWeightStrategy : public ActivityWeightStrategy {
public:
  LocalSpatialVarWeightStrategy( rc_ptr< AimsData<float> > localSpatialVarImage ) :
    ActivityWeightStrategy( ), _criterion(localSpatialVarImage) {}
  virtual ~LocalSpatialVarWeightStrategy() {}
  
  virtual float weight( Point3d& p ) 
  {
    return 1. / (*_criterion)(p) ;
  }
  
private:
  rc_ptr<AimsData<float> > _criterion ;
};

class PVEWeightStrategy : public ActivityWeightStrategy {
public:
  PVEWeightStrategy( rc_ptr<MaskIterator> maskIt, const Point3df& psf )
  {
    Point3d dims = maskIt->volumeDimension() ;
    Point3df vs = maskIt->voxelSize() ;
    _weightsImage = AimsData<float>( dims[0], dims[1], dims[2] ) ;
    _weightsImage.setSizeXYZT( vs[0], vs[1], vs[2] ) ;
    
    maskIt->restart() ;
    while( maskIt->isValid() ){
      _weightsImage( maskIt->value() ) = 1. ;
      maskIt->next() ;
    }
    
    Gaussian3DSmoothing<float> g3d(psf[0], psf[1], psf[2]) ;
    _weightsImage = g3d.doit( _weightsImage ) ;
  }
  
  virtual ~PVEWeightStrategy() {}
  
  virtual float weight( Point3d& p ){
    return _weightsImage( p ) ;
  }
  
private:
  AimsData<float> _weightsImage ;
};

class StopStrategy{
public:
  StopStrategy( int32_t minNbOfPts ) : _minNbOfPts(minNbOfPts), _iteration(0), _maxNbOfPts(0) 
  {}
  virtual ~StopStrategy() {}
  
  void restart() { _iteration = 0 ; }
  
  bool onlyMinNbOfPts( ) { return bool( _iteration <= _minNbOfPts) ; }
  virtual const AimsData<float>& smoothedMask( ) = 0 ;
  virtual bool stop( Point3d& p ) = 0 ;
  
protected:
  int32_t _minNbOfPts ;
  int32_t _iteration ;
  int32_t _maxNbOfPts ;  
};

class MaxAdmittedPVEStopStrategy : public StopStrategy {
public:
  MaxAdmittedPVEStopStrategy(  int minNbOfPts, rc_ptr<MaskIterator> maskIt, const Point3df& psf, 
			       float maxAdmittedPVEPercentage ):
    StopStrategy( minNbOfPts ), _maxAdmittedPVE( maxAdmittedPVEPercentage / 100. )
  {
    Point3d dims = maskIt->volumeDimension() ;
    Point3df vs = maskIt->voxelSize() ;
    _smoothedMask = AimsData<float>( dims[0], dims[1], dims[2] ) ;
    _smoothedMask.setSizeXYZT( vs[0], vs[1], vs[2] ) ;
    
    maskIt->restart() ;
    while( maskIt->isValid() ){
      _smoothedMask( maskIt->value() ) = 1. ;
      ++_maxNbOfPts ;
      maskIt->next() ;
    }
    
    Gaussian3DSmoothing<float> g3d(psf[0], psf[1], psf[2]) ;
    _smoothedMask = g3d.doit( _smoothedMask ) ;
    
  }
  virtual ~MaxAdmittedPVEStopStrategy() {}
  
  virtual const AimsData<float>& smoothedMask( )
  { return _smoothedMask ; } 
  
  virtual bool stop( Point3d& p ){
    if( ( _smoothedMask(p) >= 1. - _maxAdmittedPVE || _iteration < _minNbOfPts ) 
	&& _iteration < _maxNbOfPts ){
      ++_iteration ;
      return false ;
    }
    return true ;
  }
  
protected:
  AimsData<float> _smoothedMask ;
  float _maxAdmittedPVE ;
};

class NbPointsOverAdmittedPVEStopStrategy : public MaxAdmittedPVEStopStrategy {
public:
  NbPointsOverAdmittedPVEStopStrategy(  int minNbOfPts, rc_ptr<MaskIterator> maskIt, const Point3df& psf, 
					float maxAdmittedPVEPercentage ):
    MaxAdmittedPVEStopStrategy( minNbOfPts, maskIt, psf, maxAdmittedPVEPercentage ), _totalNbOfPts(0)
  {
    Point3d dims = maskIt->volumeDimension() ;
    Point3df vs = maskIt->voxelSize() ;
    _smoothedMask = AimsData<float>( dims[0], dims[1], dims[2] ) ;
    _smoothedMask.setSizeXYZT( vs[0], vs[1], vs[2] ) ;
    
    maskIt->restart() ;
    while( maskIt->isValid() ){
      _smoothedMask( maskIt->value() ) = 1. ;
      ++_maxNbOfPts ;
      maskIt->next() ;
    }
    
    Gaussian3DSmoothing<float> g3d(psf[0], psf[1], psf[2]) ;
    _smoothedMask = g3d.doit( _smoothedMask ) ;

    cout << "Threshold = " << 1. - maxAdmittedPVEPercentage/100. << endl ;
    maskIt->restart() ;
    while( maskIt->isValid() ){
      cout << _smoothedMask( maskIt->value() ) << " against " << 1. - maxAdmittedPVEPercentage/100. << endl ;
      
      if( _smoothedMask( maskIt->value() ) >= 1. - maxAdmittedPVEPercentage / 100. )
	++_totalNbOfPts ;
      maskIt->next() ;
    }
  }
  virtual ~NbPointsOverAdmittedPVEStopStrategy() {}
  
  virtual bool stop( Point3d& ){
    //cout << "iter = " << _iteration << "\ttotNbPts = " << _totalNbOfPts << "\tminNbOfPts = " << _minNbOfPts << endl ;
    if( ( _iteration < _totalNbOfPts || _iteration < _minNbOfPts ) 
	&&  _iteration < _maxNbOfPts ){
      ++_iteration ;
      return false ;
    }
    return true ;
  }
  
protected:
  AimsData<float> _smoothedMask ;
  float _maxAdmittedPVE ;
  int32_t _totalNbOfPts ;
};

class VoxelSelectionStrategy{
public:
  VoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
			  rc_ptr< AimsData<float> > criterion,
			  rc_ptr< AimsData<float> > data,
			  rc_ptr< ActivityWeightStrategy > weightStrat,
			  rc_ptr< StopStrategy > stopStrat,
			  AimsData<int32_t>& aggregationOrder ) :
    _maskIt(maskIt), _criterion(criterion), _data(data), 
    _weightStrat(weightStrat), _stopStrat( stopStrat),
    _aggregationOrder(aggregationOrder)
  {    
  }
  
  virtual ~VoxelSelectionStrategy() {}

  virtual void restart() = 0 ;
  virtual Point3d current() = 0 ;
  virtual void next() = 0 ;
  virtual void confirm( vector<float>& wMu, vector<float>& wVar, vector<float>& wMuVar ) = 0 ;
  virtual bool isValid( ) = 0 ;
  
  int32_t regionTotalNbOfPts() { return _regionNbOfPts ; }
  
  const set<Point3d, PointLess>& extractedPoints()
  { return _extractedRegionPoints ; }
  
  int32_t nbOfExtractedPoints() { return _extractedRegionPoints.size() ; }
  
  const AimsData<int32_t>& aggregationOrder()
  {
    return _aggregationOrder ;
  }
  
protected:
  bool in( const Point3d& p ) {
    if( p[0] >= 0 && p[0] < _criterion->dimX() && 
	p[1] >= 0 && p[1] < _criterion->dimY() &&
	p[2] >= 0 && p[2] < _criterion->dimZ() )
      return true ;
    return false ;
  }
  
  rc_ptr<MaskIterator> _maskIt ;
  rc_ptr< AimsData<float> >  _criterion ;
  rc_ptr< AimsData<float> > _data ;
  rc_ptr< ActivityWeightStrategy > _weightStrat ;
  rc_ptr< StopStrategy > _stopStrat ;
  AimsData<int32_t> _aggregationOrder ;
  AimsData<float> _weightsImage ;
  int32_t _regionNbOfPts ;
  set<float> _weights ;
  
  set<Point3d, PointLess> _extractedRegionPoints ;
} ;
	  

// class LMAVoxelSelectionStrategy : public VoxelSelectionStrategy{
// public:
//   LMAVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
// 			     rc_ptr< AimsData<float> > criterion,
// 			     rc_ptr< AimsData<float> > data,
// 			     const Point3df& mhs, int minNbOfPts ) ;
  
//   virtual ~LMAVoxelSelectionStrategy() {}
  
//   virtual void restart() ;
//   virtual Point3d current() ;
//   virtual void next() ;
//   virtual AimsData<float> confirm( const AimsData<float>& mu ) ;
//   bool isValid( ) ;
  
// protected:
//   multimap<float, Point3d> _orderedMinima ;
//   multimap<float, Point3d>::iterator _voxIt ;
//   bool _updated ;
//   multimap<float, Point3d> _trial ;
//   AimsData<float> _distMap ;
//   int _minNbOfPts ;
// } ;

// void 
// LMAVoxelSelectionStrategy::restart()
// {
//   int x, y, z ;
//   ForEach3d()
//   multimap<float, Point3d> orderedVoxels ;
//   if( _orderedMinima.empty() ) {
//     while( _maskIt->isValid() ){
//       Point3d p = maskIt->value() ;
//       orderedVoxels.insert( pair<float, Point3d>( (*_criterion)(p), p ) ) ;
//       _maskIt->next() ;
//     }
    
//     for( ) 
//   }
  
//   _trial = _orderedMinima ;
// }

// LMAVoxelSelectionStrategy::
// LMAVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
// 				 rc_ptr< AimsData<float> > criterion,
// 				 rc_ptr< AimsData<float> > data ) :
//   VoxelSelectionStrategy( maskIt, criterion, data ), _updated(false)
// {
//   _maskIt->restart() ;
  
//   while( _maskIt->isValid() ){
//     Point3d p = maskIt->value() ;
//     _orderedVoxels.insert( pair<float, Point3d>( (*_criterion)(p), p ) ) ;
//     _maskIt->next() ;
//   }
//   _voxIt = _orderedVoxels.begin() ;
// }

// Point3d 
// LMAVoxelSelectionStrategy::current( )
// {
//   return _voxIt->second ;
// }

// bool 
// LMAVoxelSelectionStrategy::isValid( )
// {
//   if( _voxIt == _orderedVoxels.end() )
//     return false ;
  
//   return true ;
// }

// void 
// LMAVoxelSelectionStrategy::next( )
// {
//   ++_voxIt ;
//   _updated = false ;
// }

// AimsData<float> 
// LMAVoxelSelectionStrategy::confirm( const AimsData<float>& mu )
// {
// //   cout << "LMAVoxelSelectionStrategy::confirm" << endl ;
// //   cout << "Mu : " << endl ;
// //   for( int t = 0 ; t < mu.dimX() ; ++t )
// //     cout << "\t" << mu(t) ;
// //   cout << endl ;
  
//   if( _updated )
//     return mu ;
//   Point3d p( _voxIt->second ) ;
//   _extractedRegionPoints.push_back( p ) ;
//   AimsData<float> newMu( mu.dimX() ) ;
//   for( int t = 0 ; t < _data->dimT() ; ++t )
//     newMu(t) = ( ( _extractedRegionPoints.size() - 1 ) * mu(t) + (*_data)( p[0], p[1], p[2], t ) ) 
//       / _extractedRegionPoints.size()  ;

// //   cout << "New Mu : " << endl ;
// //   for( int t = 0 ; t < mu.dimX() ; ++t )
// //     cout << "\t" << newMu(t) ;
// //   cout << endl ;

//   _updated = true ;

//   return newMu ;
// }

inline void
weightedMoments( const set<Point3d, PointLess>& points, const AimsData<float>& data,
		 rc_ptr<ActivityWeightStrategy> weightStgy, 
		 vector<float>& weightedMean, vector<float>& weightedVariance, 
		 vector<float>& weightedMeanVariance, bool computeAWeightedVariance )
{
  weightedMean = vector<float>( data.dimT(), 0. ) ; 
  weightedMeanVariance = vector<float>( data.dimT(), 0. ) ; 
  weightedVariance = vector<float>( data.dimT(), 0. ) ; 
  float weight ;
  double weightSum = 0. ;
  double sqrWeightSum = 0. ;
  Point3d p ;
  int32_t nbPts = 0 ; 
  int i = 0 ;
  for( set<Point3d, PointLess>::iterator it = points.begin() ; it != points.end() ; ++it, ++i ){
    ++nbPts ;
    p = *it ;
    weight = weightStgy->weight( p ) ;
    weightSum += weight ;
    sqrWeightSum += weight * weight ;
//     cout << "Ind " << i  << " : " ;
    for( int t = 0 ; t < data.dimT() ; ++t ){
//       cout << "\t" << data( p[0], p[1], p[2], t ) ;
      weightedMean[t] += weight * data( p[0], p[1], p[2], t ) ;
    }
//     cout << " with W = " << weight << endl ;
  }
  
  for( int t = 0 ; t < data.dimT() ; ++t )
    weightedMean[t] /= weightSum ;
  
//   cout << "wMu    = " ;
//   for( int t = 0 ; t < weightedMean.size() ; ++t )
//     cout << "\t" << weightedMean[t] ;
//   cout << endl ;
  weight = 1. ;
  i = 0 ;
  for( set<Point3d, PointLess>::iterator it = points.begin() ; it != points.end() ; ++it, ++i ){
    p = *it ;
    if( computeAWeightedVariance )
      weight = weightStgy->weight( p ) ;
//     cout << "Ind " << i << " : " ;
    for( int t = 0 ; t < data.dimT() ; ++t ){
      weightedVariance[t] += weight * sqr( data( p[0], p[1], p[2], t ) - weightedMean[t] ) ;
//       cout << "\t" << sqr( data( p[0], p[1], p[2], t ) - weightedMean[t] ) ;
    }
//     cout << " with W = " << weight << endl ;
  }
  for( int t = 0 ; t < data.dimT() ; ++t ){
    // TEMP: JUST A TEST 
    // weightedVariance[t] /= weightSum - 1. ;
    if( computeAWeightedVariance ){
      weightedVariance[t] /= weightSum - 1. ;
      weightedMeanVariance[t] = weightedVariance[t] * sqrWeightSum / sqr( weightSum ) ;
    } else {
      weightedVariance[t] /= nbPts - 1. ;
      weightedMeanVariance[t] = weightedVariance[t] / nbPts ;
    }
  }
//   cout << "wMuVar  = " ;
//   for( int t = 0 ; t < weightedMeanVariance.size() ; ++t )
//     cout << "\t" << weightedMeanVariance[t] ;
//   cout << "\t with totalWeight = " << weightSum << " and nbPts = " << nbPts << endl << endl ;
  
}


// inline void
// weightedMoments( const set<Point3d, PointLess>& points, const AimsData<float>& data,
// 		 const set<float>& weights, vector<float>& weightedMean, 
// 		 vector<float>& weightedVariance, vector<float>& weightedMeanVariance, 
// 		 bool computeAWeightedVariance )
// {
//   weightedMean = vector<float>( data.dimT(), 0. ) ; 
//   weightedMeanVariance = vector<float>( data.dimT(), 0. ) ; 
//   weightedVariance = vector<float>( data.dimT(), 0. ) ; 
//   float weight ;
//   double weightSum = 0. ;
//   double sqrWeightSum = 0. ;
//   Point3d p ;
//   int32_t nbPts = 0 ; 
//   set<Point3d, PointLess>::iterator it ;
//   set<float>::const_iterator itW ;
  
//   for( it = points.begin(), itW = weights.begin() ; 
//        it != points.end() && itW != weights.end() ; ++it, ++itW ){
//     ++nbPts ;
//     p = *it ;
//     weight = *itW ;
//     weightSum += weight ;
//     sqrWeightSum += weight * weight ;
//     for( int t = 0 ; t < data.dimT() ; ++t ){
//       weightedMean[t] += weight * data( p[0], p[1], p[2], t ) ;
//     }
//   }
  
//   for( int t = 0 ; t < data.dimT() ; ++t )
//     weightedMean[t] /= weightSum ;
  
//   weight = 1. ;
//   for( it = points.begin(), itW = weights.begin() ; 
//        it != points.end() && itW != weights.end() ; ++it, ++itW ){
//     p = *it ;
//     if( computeAWeightedVariance )
//       weight = *itW ;
//     for( int t = 0 ; t < data.dimT() ; ++t ){
//       weightedVariance[t] += weight * sqr( data( p[0], p[1], p[2], t ) - weightedMean[t] ) ;
//     }
//   }
//   for( int t = 0 ; t < data.dimT() ; ++t ){
//     // TEMP: JUST A TEST 
//     // weightedVariance[t] /= weightSum - 1. ;
//     if( computeAWeightedVariance ){
//       weightedVariance[t] /= weightSum - 1. ;
//       weightedMeanVariance[t] = weightedVariance[t] * sqrWeightSum / sqr( weightSum ) ;
//     } else {
//       weightedVariance[t] /= nbPts - 1. ;
//       weightedMeanVariance[t] = weightedVariance[t] / nbPts ;
//     }
//   }
// }


class CriterionVoxelSelectionStrategy : public VoxelSelectionStrategy {
public:
  CriterionVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
				   rc_ptr< AimsData<float> > criterion,
				   rc_ptr< AimsData<float> > data, 
				   rc_ptr< ActivityWeightStrategy > weightStrat,
				   rc_ptr< StopStrategy > stopStrat,
				   AimsData<int32_t>& aggregationOrder ) ;
  
  virtual ~CriterionVoxelSelectionStrategy() {}
  
  virtual void restart() ;
  virtual Point3d current() ;
  virtual void next() ;
  virtual void confirm( vector<float>& wMu, vector<float>& wVar, vector<float>& wMeanVar ) ;
  virtual bool isValid( ) ;
  
protected:
  multimap<float, Point3d> _orderedVoxels ;
  multimap<float, Point3d>::iterator _voxIt ;
  bool _updated ;
} ;

void 
CriterionVoxelSelectionStrategy::restart()
{
  _voxIt = _orderedVoxels.begin() ;
}

CriterionVoxelSelectionStrategy::
CriterionVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
				 rc_ptr< AimsData<float> > criterion,
				 rc_ptr< AimsData<float> > data, 
				 rc_ptr< ActivityWeightStrategy > weightStrat,
				 rc_ptr< StopStrategy > stopStrat,
				 AimsData<int32_t>& aggregationOrder ) :
  VoxelSelectionStrategy( maskIt, criterion, data, weightStrat, stopStrat, aggregationOrder ), 
  _updated(false)
{
  _maskIt->restart() ;
  if( _aggregationOrder.dimX() != data->dimX() || _aggregationOrder.dimX() != data->dimY() ||  
      _aggregationOrder.dimX() != data->dimZ() ){
    _aggregationOrder = AimsData<int32_t>( data->dimX(), data->dimY(), data->dimZ() ) ;
    _aggregationOrder.setSizeXYZT(data->sizeX(), data->sizeY(), data->sizeZ() ) ;
  }
  while( _maskIt->isValid() ){
    Point3d p = maskIt->value() ;
    _orderedVoxels.insert( pair<float, Point3d>( (*criterion)(p), p ) ) ;
    _maskIt->next() ;
  }
    
  //cout << "Map created with size" << _orderedVoxels.size() << endl ;
  _voxIt = _orderedVoxels.begin() ;
}

Point3d 
CriterionVoxelSelectionStrategy::current( )
{
  return _voxIt->second ;
}

bool 
CriterionVoxelSelectionStrategy::isValid( )
{
  //cout << "Entering is valid" << endl ;
  if( _voxIt == _orderedVoxels.end() )
    return false ;
  
  //cout << "Still in region" << endl ;
  if( _stopStrat->stop( _voxIt->second ) )
    return false ;
  
  return true ;
}

void 
CriterionVoxelSelectionStrategy::next( )
{
  ++_voxIt ;
  _updated = false ;
}

void
CriterionVoxelSelectionStrategy::confirm( vector<float>& mu, vector<float>& var, 
					  vector<float>& muVar )
{
  if( _updated )
    return ;
  Point3d p( _voxIt->second ) ;
  _extractedRegionPoints.insert( p ) ;
  
  weightedMoments( _extractedRegionPoints, *_data,
		   _weightStrat, 
		   mu, var, muVar, false ) ;
//   weightedMoments( _extractedRegionPoints, *_data,
// 		   _weights, 
// 		   mu, var, muVar, false ) ;
  
  _aggregationOrder( p ) = _extractedRegionPoints.size() ;

  _updated = true ;
}

class DistFromBordersVoxelSelectionStrategy : public CriterionVoxelSelectionStrategy {
public:
  DistFromBordersVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
					 rc_ptr< AimsData<float> > criterion,
					 rc_ptr< AimsData<float> > data, 
					 const Point3df& voxelSize, 
					 rc_ptr< ActivityWeightStrategy > weightStrat,
					 rc_ptr< StopStrategy > stopStrat, 
					 AimsData<int32_t>& aggregationOrder ) ;
  
  virtual ~DistFromBordersVoxelSelectionStrategy() {}
    
protected:
} ;

DistFromBordersVoxelSelectionStrategy::
DistFromBordersVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
				       rc_ptr< AimsData<float> > criterion,
				       rc_ptr< AimsData<float> > data, 
				       const Point3df& voxelSize, 
				       rc_ptr< ActivityWeightStrategy > weightStrat,
				       rc_ptr< StopStrategy > stopStrat,
				       AimsData<int32_t>& aggregationOrder ) :
  CriterionVoxelSelectionStrategy( maskIt, criterion, data, weightStrat, stopStrat, aggregationOrder )
{
  maskIt->restart() ;

  if( _aggregationOrder.dimX() != data->dimX() || _aggregationOrder.dimX() != data->dimY() ||  
      _aggregationOrder.dimX() != data->dimZ() ){
    _aggregationOrder = AimsData<int32_t>( data->dimX(), data->dimY(), data->dimZ() ) ;
    _aggregationOrder.setSizeXYZT(data->sizeX(), data->sizeY(), data->sizeZ() ) ;
  }

  AimsData<short> regionMask( maskIt->volumeDimension()[0], 
			      maskIt->volumeDimension()[1], 
			      maskIt->volumeDimension()[2], 1, 5 ) ;
  regionMask.setSizeXYZT( voxelSize[0], 
			  voxelSize[1], 
			  voxelSize[2] ) ;
  while( maskIt->isValid() ){
    regionMask(maskIt->value() ) = 1 ;
    maskIt->next() ;
  }
  
  AimsData<float> distMap = AimsFloatChamferDistanceMap( regionMask, AIMS_CHAMFER_INSIDE,
							 5, 5, 5, 50. ) ;
  maskIt->restart() ;
  while( maskIt->isValid() ){
    Point3d p = maskIt->value() ;
    _orderedVoxels.insert( pair<float, Point3d>( -distMap(p) , p ) ) ;
    
    _maskIt->next() ;
  }
  _voxIt = _orderedVoxels.begin() ;
}

class CompositePVECriterionVoxelSelectionStrategy : public CriterionVoxelSelectionStrategy {
public:
  CompositePVECriterionVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
					       rc_ptr< AimsData<float> > criterion,
					       rc_ptr< AimsData<float> > data, 
					       const Point3df& voxelSize, 
					       rc_ptr< ActivityWeightStrategy > weightStrat,
					       rc_ptr< StopStrategy > stopStrat, 
					       AimsData<int32_t>& aggregationOrder,
					       const Point3df& psf ) ;
  
  virtual ~CompositePVECriterionVoxelSelectionStrategy() {}
    
protected:
  
} ;

CompositePVECriterionVoxelSelectionStrategy::CompositePVECriterionVoxelSelectionStrategy( rc_ptr<MaskIterator> maskIt,
					       rc_ptr< AimsData<float> > criterion,
					       rc_ptr< AimsData<float> > data, 
					       const Point3df& voxelSize, 
					       rc_ptr< ActivityWeightStrategy > weightStrat,
					       rc_ptr< StopStrategy > stopStrat, 
					       AimsData<int32_t>& aggregationOrder,
					       const Point3df& psf ) :
  CriterionVoxelSelectionStrategy( maskIt, criterion, data, weightStrat, stopStrat, aggregationOrder )
{
  Point3d dims = maskIt->volumeDimension() ;
  Point3df vs = maskIt->voxelSize() ;
  AimsData<float> smoothedMask( dims[0], dims[1], dims[2] ) ;
  smoothedMask.setSizeXYZT( vs[0], vs[1], vs[2] ) ;
  
  AimsData<float> selCritImg( dims[0], dims[1], dims[2] ) ;
  selCritImg.setSizeXYZT( vs[0], vs[1], vs[2] ) ;
  
  maskIt->restart() ;
  float critMin = -1. ;
  bool firstPass = true ;
  int32_t maxNbOfPts = 0 ;
  while( maskIt->isValid() ){
    if( firstPass || (*criterion)( maskIt->value() ) < critMin ){
      critMin = (*criterion)( maskIt->value() ) ;
      firstPass = false ;
    }
    //cout << "crit value is " << (*criterion)( maskIt->value() ) << endl ;
    smoothedMask( maskIt->value() ) = 1. ;
    ++maxNbOfPts ;
    maskIt->next() ;
  }
  
  Gaussian3DSmoothing<float> g3d(psf[0], psf[1], psf[2]) ;
  smoothedMask = g3d.doit( smoothedMask ) ;
  
  _maskIt->restart() ;
  if( _aggregationOrder.dimX() != data->dimX() || _aggregationOrder.dimX() != data->dimY() ||  
      _aggregationOrder.dimX() != data->dimZ() ){
    _aggregationOrder = AimsData<int32_t>( data->dimX(), data->dimY(), data->dimZ() ) ;
    _aggregationOrder.setSizeXYZT(data->sizeX(), data->sizeY(), data->sizeZ() ) ;
  }
  while( _maskIt->isValid() ){
    Point3d p = maskIt->value() ;
    //cout << "inserting p = " << p << "with smoo mask val = " << smoothedMask(p) 
    //     << " & crit = " << (*criterion)(p) << " & voxOrdCrit = " <<  std::max( float(0.), std::min( float(1.), float(1. - smoothedMask(p)) ) ) * std::max( float(0.), float( (*criterion)(p) - critMin ) )
    //     << " & first = "<< std::max( float(0.), std::min( float(1.), float(1. - smoothedMask(p)) ) ) << " & second = " << std::max( float(0.), float( (*criterion)(p) - critMin ) ) << endl ;
    _orderedVoxels.insert( pair<float, Point3d>( std::max( float(0.), std::min( float(1.), float(1. - smoothedMask(p)) ) ) * std::max( float(0.), float( (*criterion)(p) - critMin ) ), p ) ) ;
    selCritImg(p) = std::max( float(0.), std::min( float(1.), float(1. - smoothedMask(p)) ) ) * std::max( float(0.), float( (*criterion)(p) - critMin ) ) ;
    _maskIt->next() ;
  }
  
  //Writer< AimsData<float> > wriSCI("slCritImg.ima") ;
  //wriSCI.write( selCritImg ) ;
  //cout << "Map created with size" << _orderedVoxels.size() << endl ;
  _voxIt = _orderedVoxels.begin() ;
}

float
regionGrowth( rc_ptr<VoxelSelectionStrategy> voxSelStgy,
	      rc_ptr< AimsData<float> > data,
	      const vector<float>& normFactor,
	      int nbMinPoints,
	      bool itRecons,
	      set<Point3d, PointLess>& extractedPoints,
	      ostream& os )
{
  float sig2ByN = 0., bestSig2ByN = 0., sigma2 = 0.  ;
  AimsData<double> err2(data->dimT()) ;
  AimsData<float> mu( data->dimT() ) ;
  vector<float> wMu( data->dimT(), 0. ), wVar( data->dimT(), 0. ), wMuVar(data->dimT(), 0.) ;
  vector<float> bestWMu( data->dimT(), 0. ), bestWVar( data->dimT(), 0. ), 
    bestWMuVar(data->dimT(), 0.) ;
  
  Point3d pCurr = voxSelStgy->current( ) ;
  bool doContinue = false ;
  //cout << "Agreg until min nb of points reached" << endl ;
  //cout << "is valid ? " << ( voxSelStgy->isValid() ? "Yes" : "No" )  << endl ;
  //cout << "Min nb of points : " << nbMinPoints << endl ;
  if(! voxSelStgy->isValid() ) 
    return -1. ;
  
  for( int n = 0 ; n < nbMinPoints && voxSelStgy->isValid() ; ++n ){
    cout << "N = " << n + 1 << " : inserting automatically " << voxSelStgy->current() << endl ;
    voxSelStgy->confirm( wMu, wVar, wMuVar ) ;
    voxSelStgy->next() ;
    
    
    bestSig2ByN = sig2ByN = weightedMeanOverTime( wMuVar, wMu, normFactor, itRecons ) ;
    cout << "Sig2 / N is " <<  sig2ByN << endl << endl ;
  }
  int32_t safetyRange =  voxSelStgy->regionTotalNbOfPts() / 50 + 2 ;
  int32_t nbOfPtsAfterMin = 0 ;
  
  //cout << "free agreg" << endl ;
  while( voxSelStgy->isValid() ){
//   while( nbOfPtsAfterMin > safetyRange ){
    doContinue = false ;
    if( bestSig2ByN > sig2ByN ){
      bestSig2ByN = sig2ByN ;
      nbOfPtsAfterMin = 0 ;
    } else {
      ++nbOfPtsAfterMin ;
    }
    voxSelStgy->confirm( wMu, wVar, wMuVar ) ;
    
    sigma2 = weightedMeanOverTime( wVar, wMu, normFactor, itRecons )  ;
    sig2ByN = weightedMeanOverTime( wMuVar, wMu, normFactor, itRecons )  ;
    
    //sig2ByN = sigma2 / voxSelStgy->nbOfExtractedPoints() ;
    voxSelStgy->next() ;
    
    if( !voxSelStgy->isValid() )
      break ;
//     cout <<  sigma2 << "\t" << sig2ByN << endl << endl  << endl  ;
//     char c ;
//     cin >> c ;
  
    os << sigma2 << "\t" << sig2ByN << endl ;
  }
  extractedPoints = voxSelStgy->extractedPoints() ;
  return bestSig2ByN ;
}

	  


template<class T>
static bool doit( Process &, const string &, Finder & );

class VoiStat : public Process
{
public:
  VoiStat( const string& regionsFile, Motion & m, const string & fout, const string& crit,
	   const string& selMethod, const string& weightMethod, const string& stopMethod, float pveTolerance,
	   const Point3df& psf, short minNbOfPts = 50, bool itRec = false );
  
private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );
  
  string	  regionsFile;
  Motion	  motion;
  string	  fileoutput;
  string          criterionFile ;
  string          selMethod ;
  string          weightMethod ;
  string          stopMethod ;
  float           pveTolerance ;
  Point3df        scannerPSF ;
  short           minNbOfPoints ;
  bool            iterativeRecons ;
};


VoiStat::VoiStat( const string& regFile, Motion & m, const string & fout, const string& crit,
		  const string& selMeth, const string& weightMeth, const string& stopMeth, 
		  float pveTol, const Point3df& psf, short minNbOfPts, bool itRec )
  : Process(), regionsFile(regFile), motion( m ), fileoutput( fout ), criterionFile(crit),
    selMethod(selMeth), weightMethod(weightMeth), stopMethod(stopMeth), pveTolerance(pveTol),
    scannerPSF(psf), minNbOfPoints(minNbOfPts), iterativeRecons(itRec)
{
  registerProcessType( "Volume", "FLOAT", &doit<float> );
}
  
  
template<class T>
bool doit( Process & p, const string & fname, Finder & f )
{
  rc_ptr< AimsData<T> >	data( new AimsData<T>() );
  Reader<AimsData<T> >	r( fname );
  string		format = f.format();
  
  const PythonHeader *ph = 
     dynamic_cast<const PythonHeader *>( f.header() );
  
  // try to use memory mapping
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
					   DataSource::none(), false, 
					   0.01 ) ) ;
  
  if( !r.read( *data, 0, &format ) )
    return( false );
  
  VoiStat & vs  = (VoiStat&) p ;
  const PythonHeader * header = dynamic_cast<const PythonHeader *>( data->header() ) ;
  
  vector<int> durationTime ;
  vector<int> startTime ;
  float isotopeHalfLife ;

  if ( !header->getProperty("duration_time", durationTime ) ){
    cerr << "No duration time !" << endl ;
    throw runtime_error("No duration time !") ;
  }
  if ( !header->getProperty("start_time", startTime ) ){
    cerr << "No start time !" << endl ;
    throw runtime_error("No start time !") ;
  }
  
  if ( !header->getProperty("isotope_halflife", isotopeHalfLife ) ){
    cerr << "No isotope halflife !" << endl ;
    throw runtime_error("No isotope halflife !") ;
  }
  
  
  vector<float> normFactor( data->dimT() ) ;
  for(int t = 0 ; t < data->dimT() ; ++t )
    normFactor[t] = durationTime[t] / 1000. * exp( -(startTime[t] + durationTime[t] / 2. ) 
						    / (1000. * isotopeHalfLife /*9461*/) ) ; 
  
  rc_ptr< AimsData<float> > criterion( new AimsData<float>() ) ;
  
  if( vs.selMethod == "Criterion" || vs.selMethod == "Hybrid" ){
    if( vs.criterionFile == "" )
      throw runtime_error ("A multistructure local presence criterion image must be provided"
			   " for the Criterion mode !") ;
    Reader< AimsData<float> > reader( vs.criterionFile ) ;
    reader.read(*criterion) ;
  }
  
  rc_ptr<VoxelSelectionStrategy> voxSelStgy ;
  rc_ptr<ActivityWeightStrategy> weightStgy ;
  rc_ptr<StopStrategy> stopStgy ;
  
  //cout << "getting roi it " << endl ;
  carto::rc_ptr< RoiIterator > roiIt = getRoiIterator( vs.regionsFile ) ;
  
  //cout << "getting voxel size" << endl ;
  Point3df voxelSize = roiIt->voxelSize() ;
  float voxelVolume = roiIt->voxelSize()[0] *  roiIt->voxelSize()[1] *  roiIt->voxelSize()[2] ;
  
  //cout << "got voxel size" << endl ;
  
  map< string, set<Point3d, PointLess> > extPtsByRegion ;
  map< string, float > regionVolume ;
  ofstream os("forExcell.txt") ;
  
  AimsData<int32_t> voxOrder( data->dimX(), data->dimY(), data->dimZ(), roiIt->count() ) ;
  voxOrder.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ(), 1.0 ) ;
  AimsData<int32_t> voxOrderForRegion( data->dimX(), data->dimY(), data->dimZ() ) ;
  voxOrderForRegion.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ() ) ;
  
  AimsData<float> smoothedMasks( data->dimX(), data->dimY(), data->dimZ(), roiIt->count() ) ;
  smoothedMasks.setSizeXYZT( data->sizeX(), data->sizeY(), data->sizeZ(), 1.0 ) ;
  
  int i = 0 ;
  while( roiIt->isValid() ){
    rc_ptr<MaskIterator> maskIt = roiIt->maskIterator() ;
    //cout << "Doing " << roiIt->regionName() << endl ;
    os << roiIt->regionName() << endl ;
    int nbPts = 0 ;
    while( maskIt->isValid() ){
      ++nbPts ;
      maskIt->next() ;
    }
    regionVolume[roiIt->regionName()] = nbPts * voxelVolume  ;
//     if( weightStgy ){
//       delete weightStgy ;
//       weightStgy = 0 ;
//     }
    if( vs.weightMethod == "Criterion" ){
      weightStgy = rc_ptr< ActivityWeightStrategy >( new LocalSpatialVarWeightStrategy( criterion ) ) ;
    }else if( vs.weightMethod == "PVE" ){
      weightStgy = rc_ptr< ActivityWeightStrategy >( new PVEWeightStrategy( maskIt, vs.scannerPSF) ) ;
    } else {
      throw runtime_error("Bad choice for the voxels' activity weight method !") ;  
    }
    
//     if( stopStgy ){
//       delete stopStgy ;
//       stopStgy = 0 ;
//     }
    if( vs.stopMethod == "PVE" ){
      stopStgy = rc_ptr<StopStrategy>( new MaxAdmittedPVEStopStrategy( vs.minNbOfPoints, maskIt, 
								       vs.scannerPSF, 
								       vs.pveTolerance ) ) ;
    }else if( vs.stopMethod == "NbPtsPVE" ){
      stopStgy = rc_ptr<StopStrategy>( new NbPointsOverAdmittedPVEStopStrategy( vs.minNbOfPoints, 
										maskIt, 
										vs.scannerPSF, 
										vs.pveTolerance ) ) ;
    } else {
      throw runtime_error("Bad choice for the region growth stop method !") ;  
    }

//     if( voxSelStgy ){
//       delete voxSelStgy ;
//       voxSelStgy = 0 ;
//     }
    if( vs.selMethod == "Criterion" ){
      voxSelStgy = 
	rc_ptr<VoxelSelectionStrategy>( new CriterionVoxelSelectionStrategy( maskIt, criterion, data, 
									     weightStgy, stopStgy, 
									     voxOrderForRegion ) ) ;
    }else if( vs.selMethod == "DistMap" ){
      voxSelStgy = 
	rc_ptr<VoxelSelectionStrategy>( new DistFromBordersVoxelSelectionStrategy( 
						 maskIt, criterion, 
						 data, voxelSize, 
						 weightStgy, 
						 stopStgy, 
						 voxOrderForRegion)) ;
    } else if ( vs.selMethod == "Hybrid" ) {
      //Writer< AimsData<float> > wri( "critForHybrid.ima") ;
      //wri.write( *criterion ) ;
      
      voxSelStgy = 
	rc_ptr<VoxelSelectionStrategy>( new CompositePVECriterionVoxelSelectionStrategy( 
						 maskIt, criterion, 
						 data, voxelSize, 
						 weightStgy, 
						 stopStgy, 
						 voxOrderForRegion, 
						 vs.scannerPSF ) ) ;
      
    } else
      throw runtime_error("Bad choice for the voxel selection order method !") ;
    
    
    int x, y, z ;
    ForEach3d( stopStgy->smoothedMask(), x, y, z )
      smoothedMasks(x, y, z, i) = stopStgy->smoothedMask()(x, y, z) ;
    
    regionGrowth( voxSelStgy, data,
		  normFactor, vs.minNbOfPoints,
		  vs.iterativeRecons, extPtsByRegion[ roiIt->regionName() ], os  ) ;
    voxOrderForRegion = voxSelStgy->aggregationOrder() ;
    //Writer< AimsData<int32_t> > wri("voxOrderForRegion.ima") ;
    //wri.write( voxOrderForRegion ) ;
    ForEach3d( voxOrderForRegion, x, y, z ){
      voxOrder(x, y, z, i) = voxOrderForRegion( x, y, z ) ;
    }
    ++i ;
    roiIt->next() ;
  }
  //Writer< AimsData<float> > wriSM("smoothedMasks.ima") ;
  //wriSM.write( smoothedMasks ) ;
  
  //Writer< AimsData<int32_t> > wri("voxOrder.ima") ;
  //wri.write( voxOrder ) ;

  os.close() ;

  map<string, vector<float> > min ;
  map<string, vector<float> > max ;
  map<string, vector<float> > means ;
  map<string, vector<float> > stdDevs ;
  map<string, vector<float> > confidenceOnMean ;
  
  string unit ;
  ofstream ofs(vs.fileoutput.c_str()) ;
  
  if( !ph )
    cerr << "image series header does not seem correct (?)\n" ;
  else if( ph->getProperty( "image_unit", unit ) ){
    ofs << "Image unit : " << unit << endl ;
    ofs << "Volume unit : mm3" << endl ;
  }
  
  string separ = "|";
  ofs << "\t" << "RoiName" << separ  
      << "\t" << "StTime" << separ 
      << "\t" <<"DuTime" << separ 
      << "\t" << "Volume" << separ
      << "\t" << "SubVolume" << separ
      << "\t" << "Mean" << separ 
      << "\t" << "StdDev" << separ 
      << "\t" << "Min" << separ 
      << "\t" << "Max" << separ 
      << "\t" << "MeanEstimationErrorStdDev" << endl ;
  
  for(map< string, set<Point3d, PointLess> >::iterator itR = extPtsByRegion.begin() ;
	itR != extPtsByRegion.end() ; ++itR){
    vector<float> sum( data->dimT(), 0. ) ;
    vector<float> sum2( data->dimT(), 0. ) ;
    
    means[itR->first] = vector<float>( data->dimT() ) ;
    stdDevs[itR->first] = vector<float>( data->dimT() ) ;
    confidenceOnMean[itR->first] = vector<float>( data->dimT() ) ;
    min[itR->first] = vector<float>( data->dimT() ) ;
    max[itR->first] = vector<float>( data->dimT() ) ;
    for( int t = 0 ; t < data->dimT() ; ++t ){
      for( set<Point3d, PointLess>::iterator it = itR->second.begin() ; 
	   it != itR->second.end() ; ++it){
	Point3d p = *it ;
	float val = (*data)( p[0], p[1], p[2], t ) ;
	sum[t] += val ;
	sum2[t] += val * val ;
	if( val < min[itR->first][t] || it == itR->second.begin() )
	  min[itR->first][t] = val ;
	if( val > max[itR->first][t] || it == itR->second.begin() )
	  max[itR->first][t] = val ;
      }
    means[itR->first][t] = sum[t] / itR->second.size() ;
    stdDevs[itR->first][t]
	= ::sqrt( itR->second.size() / (itR->second.size() - 1 ) 
		* ( sum2[t] / itR->second.size() - sqr(means[itR->first][t]) ) ) ;
      confidenceOnMean[itR->first][t] = stdDevs[itR->first][t] / ::sqrt( itR->second.size() ) ;
      
      ofs << "\t" << itR->first << separ  
	  << "\t" << startTime[t] << separ 
	  << "\t" << durationTime[t] << separ 
	  << "\t" << regionVolume[itR->first] << separ
	  << "\t" << itR->second.size() * voxelVolume << separ
	  << "\t" << means[itR->first][t] << separ 
	  << "\t" << stdDevs[itR->first][t] << separ 
	  << "\t" <<  min[itR->first][t] << separ 
	  << "\t" << max[itR->first][t] << separ 
	  << "\t" << confidenceOnMean[itR->first][t] << endl ;
    }
  }
  ofs.close() ;

  return( true );
}

double
widthHalfHeightToSigma( double widthHalfHeight )
{
  return widthHalfHeight / ::sqrt( 2. * ::log(2.) ) ;
}

int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;

  try
  {
    string	fileseries, filevoi;
    string	filemotion, filehierarchy, fileout, filecriterion = "", fileroiselector = "all";
    string	fileroiseluser, selectionOrderMethod = "Hybrid", 
      weightMethod = "Criterion", stopMethod = "NbPtsPVE" ;
    string	voitype = "arg";
    string      scannerType = "FOCUS", isotope = "18F", distToCenter = "far" ;
    float       psfXY = -1., psfZ = -1. ;
    bool	itRec = false ; 
    int32_t     minNbOfPts = 50 ;
    float       pveTolerance = 10. ;
    AimsApplication	app( argc, argv, 
                         "Compute ordinary stat from a RoiArg and and Image (Warning: "
                         "DOUBLE images are converted to FLOAT so there si a possible loss "
                         "of information), but optimized by the LMA"
			 "algorithm used for segmentation" );
    
    app.addOption( fileseries, "-i", "input dynamic data" );
    app.addOption( filevoi, "-v", "file containing roi information see "
                   "(--voitype)" );
    app.addOption( fileout, "-o", "stat result filename" );
    app.addOption( filecriterion, "-c", "criterion image produced by AimsDynamicImageSegmentation", true );
    app.addOption( selectionOrderMethod, "--selOrder", 
		   "method used to select voxel order (Default : PVE) : \n"
		   "\tCriterion : from the lowest criterion value to the highest\n"
		   "\tPVE : from the lowest affectation by partial volume effect to the highest\n"
		   "\tHybrid : hybrid method between between PVE and Criterion\n"
		   "\tLMA : from the lowest energy of the minimal path joining the voxel with the\n"
		   "\tnearest criterion minimum to the highest\n", 
		   true );
    app.addOption( weightMethod, "--weights", 
		   "weights applied to the activity concentration values (Default : Criterion) : \n"
		   "\tCriterion : 1/sig2, where sig2 is the spatial variance in the voxel's neighborhood\n"
		   "\tPVE : value of the mask smoothed by the scanner's PSF\n"
		   "\tLMA : Energy of the minimal path from the voxel to the nearest criterion minimum\n", 
		   true );
    app.addOption( stopMethod, "--stopMethod", 
		   "method determinating when to stop the region growth (Default : PVE) : \n"
		   "\tPVE : when current voxel's PVE is over the tolerated PVE parameter \n"
		   "\tNbPtsPVE : while sub region cardinal is under the number of voxels whose PVE affection"
		   " is under the tolerated PVE parameter\n", 
		   true );
    app.addOption( pveTolerance, "--pveTolerance", "tolerance on the percentage of signal in a voxel than comes"
		   " from other organs, in % (default : 10 (%))", true ) ;
    app.addOption( scannerType, "--imager", "Scanner type (FOCUS, HRRT, HR+) (default: FOCUS)", true ) ;
//     app.addOption( isotope, "--isotope", "Isotope type (18F, Other) (default: 18F)" ) ;
    app.addOption( distToCenter, "--dist", "Maximal qualitative distance to center of the scanner's FOV\n"
		   "\t(far, medium, close) (default: far)", true ) ;
    app.addOption( psfXY, "--psfXY", "Scanner real resolution in transaxial plane\n"
		   "\t(necessary only for acquisition other than on HR+, HHRT or FOCUS with 18F)", 
		   true );
    app.addOption( psfZ, "--psfZ", "Scanner real resolution on scanner's axis\n"
		   "\t(necessary only for acquisition other than on HR+, HHRT or FOCUS with 18F)", 
		   true );
    app.addOption( filemotion, "-d", "displacement file from functional image" 
                   "to ROI. default val : Identity", true );
//     app.addOption( minVol, "--minVolume", 
// 		   "minimum volume of the extracted sub regions. If this parameter is set,\n"
// 		   "--minNbOfPoints is ignored (default : -1. (No volume minimum) )", true ) ;
    app.addOption( minNbOfPts, "--minNbOfPoints", 
		   "minimum number of points used to compute stats (default : 1)", true ) ;
    app.addOption( itRec, "--it", 
		   "Precises if the image is reconstructed using an interative algorithm ?", true ) ;
    app.alias( "--input", "-i" );
    app.alias( "--voi", "-v" );
    app.alias( "--displ", "-d" );
    app.alias( "--output", "-o" );
    
    app.initialize();
    
    Point3df psf ;
    float sig ;
    cout << "If isotope is not 18F, you should provide for better results the"
      " precise measurements of the real spatial"
      " resolution for this isotope. It will be considered equal to the one for 18F"<< endl ;
    isotope = "18F" ;
    if( psfXY != -1. ){
      if( psfZ == -1. )
	psfZ = psfXY ;
      psf = Point3df( psfXY, psfXY, psfZ ) ;
    }else{
      if( scannerType == "HR+" ){
	psf = Point3df( 2.83231, 2.83231, 3.0 ) ;
      } else {
	if( scannerType == "FOCUS" ){
	  if( distToCenter == "close" )
	    sig = widthHalfHeightToSigma( 1.6 ) ;
	  else if( distToCenter == "medium" )
	    sig = widthHalfHeightToSigma( 2 ) ;
	  else 
	    sig = widthHalfHeightToSigma( 2.8 ) ;
	  psf = Point3df( 0.7, 0.7, 0.7 ) ;
	} else if( scannerType == "HRRT" ){
	  if( distToCenter == "close" )
	    sig = widthHalfHeightToSigma( 2.4 ) ;
	  else if( distToCenter == "medium" )
	    sig = widthHalfHeightToSigma( 2.9 ) ;
	  else 
	    sig = widthHalfHeightToSigma( 3.4 ) ;
	} else 
	  throw runtime_error( "You have specified no scanner nor" ) ;
	psf = Point3df( sig, sig, sig ) ;
	cout << "Sigma of the psf fuction is " << sig << " mm" << endl ;
      }
    }
    
    bool criterionAvailable = false ;
    if( filecriterion != "" ){
      try{
	AimsData<float> criterion ;
	Reader< AimsData<float> > readerCrit( filecriterion ) ;
	readerCrit.read( criterion ) ;
	criterionAvailable = true ;
      } catch( exception& e ){
	throw runtime_error( "Bad organ core criterion file name !") ;
      };
    }
    
    //Read Motion info =========================================================
    Motion motion;  // set with Identity
    if( !filemotion.empty() )
      {
        MotionReader mrd( filemotion );
        mrd >> motion;
      }
        
    // Get a Stat Actuator Object with chosen interpolator and perf. stats
    
    VoiStat proc( filevoi, motion, fileout, filecriterion, selectionOrderMethod, 
		  weightMethod, stopMethod, pveTolerance, psf, minNbOfPts, itRec );
    if( !proc.execute( fileseries ) )
      {
        cerr << "Could not process\n";
        return( 1 );
      }
  }
  catch ( user_interruption& e )
  {
  }
  catch ( std::exception& e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;

  }

  return result;
}
