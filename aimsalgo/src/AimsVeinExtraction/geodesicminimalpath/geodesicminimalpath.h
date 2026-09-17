
#ifndef AIMS_GEODESICMINIMALPATH_GEODESICMINIMALPATH_H
#define AIMS_GEODESICMINIMALPATH_GEODESICMINIMALPATH_H

#include "../volume/volume.h"
#include <aims/vector/vector.h>
#include <map>

static const float Infinite = 1E10 ;

class GeodesicMinimalPath
{
  enum state {
    far,
    trial,
    alive
  } ;
  
public:
  GeodesicMinimalPath( ) ;
  GeodesicMinimalPath( 	const Volume< float >& potential, 
			const Point3d& initialPoint,
			float distanceStep = .01,
			bool lowerValueInaccessible = false ) ;
  ~GeodesicMinimalPath( ) ;
  
  void setInitialPoint( const Point3d& initialPoint ) ;
  void setPotential( const Volume<float>& potential ) ;
  void setDistanceStep( float distanceStep ) ;
  std::list < Point3d > getMinimalPath( const Point3d& finalPoint ) ;
  Volume<float> getMinimalActionMap( ) { 
    if(mustRecompute_){
      initMap() ;
      computeMap( ) ;
    }
    return *minimalActionMap_ ; 
  }

private:
  void computeMap( ) ;
  struct energyCompare{
    bool operator()(float a, float b) const
      { return a < b ; }
  };
  void initMap( ) ;
  double solveEquation( const Point3d& trialPoint ) ;
  std::list < Point3d > backtraceMinimalPath( const Point3d& finalPoint ) ;
	
  bool mustRecompute_ ;
  float distanceStep_ ;
  Point3d initialPoint_ ;
  std::multimap<float, Point3d, energyCompare> trialPoints_ ;
  Volume<float>* potential_ ;
  Volume<int>* state_ ; 
  Volume<float>* minimalActionMap_ ;
  bool lowerValueInaccessible_ ; 
} ;

#endif
