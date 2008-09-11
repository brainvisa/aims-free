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
    bool operator()(float a, float b)
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
