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


#include "geodesicminimalpath.h"
#include <aims/data/data.h>
#include <aims/io/io_g.h>
#include <iostream>

using namespace std;

/*
 *
 * GeodesicMinimalPath
 *
 * Implementation of Laurent D Cohen and Ron Kimmel : 
 * "Global Minimum for Active Contour model : A Minimal Path Approach"
 *
 */

/*
 * Default Constructor
 * 
 * Potential, InitialPoint and DistanceStep must be defined before computing minimal path
 *
 */
 
using namespace aims ;
 
GeodesicMinimalPath::GeodesicMinimalPath( ) :
  mustRecompute_(true), distanceStep_(0.1), initialPoint_(Point3d(-1, -1, -1)), 
  potential_(0), state_(0), minimalActionMap_(0)
{ }

/*
 * Constructor
 */
GeodesicMinimalPath::GeodesicMinimalPath( const Volume<float>& potential, 
					  const Point3d& initialPoint,
					  float distanceStep,
					  bool lowerValueInaccessible )
  :  mustRecompute_(true), distanceStep_(distanceStep), initialPoint_(initialPoint), 
  lowerValueInaccessible_(lowerValueInaccessible) {

  //cout << "potential size " << potential.size() << endl ;
//   int c ;
//   cin >> c ;
  potential_ = new Volume<float>( potential ) ;
  state_ = new Volume<int>( Point3d( potential_->size()[0],
				     potential_->size()[1],
				     potential_->size()[2] ) ) ;
  minimalActionMap_ = new Volume<float>( Point3d( potential_->size()[0],
						potential_->size()[1],
						potential_->size()[2] ) ) ;
}


/*
 * Destructor
 */

GeodesicMinimalPath::~GeodesicMinimalPath( )
{
  if( potential_ )
    delete potential_ ;
  if(state_)
    delete state_ ;
  if( minimalActionMap_ )
    delete minimalActionMap_ ;
}

/*
 * setInitialPoint()
 *
 * InitialPoint setting : necessary before computing minimal path.
 */
void
GeodesicMinimalPath::setInitialPoint( const Point3d& initialPoint ){
  if ( initialPoint_ == initialPoint )
    return ;
  initialPoint_ = initialPoint ;
   
  mustRecompute_ = true ;
}

/*
 * setPotential()
 *
 * Potential setting : necessary before computing minimal path.
 *
 * Must obviously be low where the minimal path should path.
 */
void
GeodesicMinimalPath::setPotential( const Volume<float>& potential ){
  if( potential_ == &potential ) 
    return ;
  
  if( potential_ )
    delete [] potential_ ;
  potential_ = new Volume<float>( potential ) ;
  if( state_ )
    delete [] state_ ;
  state_ = new Volume<int>( Point3d( potential_->size()[0],
				     potential_->size()[1],
				     potential_->size()[2] ) ) ;

  if( minimalActionMap_ )
    delete []minimalActionMap_ ;
  minimalActionMap_ = new Volume<float>( Point3d( potential_->size()[0],
						  potential_->size()[1],
						  potential_->size()[2] ) ) ;
  mustRecompute_ = true ;
  initMap( ) ;
}

/*
 * setDistanceStep()
 *
 * Distance Step setting : necessary before computing minimal path.
 *
 * Only regularization parameter of active contour model by Minimal Path approach.
 *
 */
void 
GeodesicMinimalPath::setDistanceStep( float distanceStep ) {
  
  if( distanceStep > 1. ) distanceStep_ = 1. ; 
  else if( distanceStep <= 0. ) distanceStep_ = 0.000001 ;
  else distanceStep_ = distanceStep ;
  
  mustRecompute_ = true ;
}

/*
 * initMap()
 *
 * Initialize the active contour model 
 *
 */
void
GeodesicMinimalPath::initMap( ){

  ASSERT( potential_ != 0 ) ;

  //cout << "\tinit " << initialPoint_ <<  "\tsize " <<  potential_->size() ; 

  ASSERT( initialPoint_[0] >=0 && initialPoint_[0] < potential_->size()[0] &&
	  initialPoint_[1] >=0 && initialPoint_[1] < potential_->size()[1] &&
	  initialPoint_[2] >=0 && initialPoint_[2] < potential_->size()[2] ) ;
  
  // Potential is normalized : value rang is set to [0, 1]
  potential_->normalize( ) ;

  int histogram[101] ;
  for ( int  i = 0 ; i < 101 ; i++ ) histogram[i] = 0 ; 
  // All voxels are set to far with an infinite energy value
  
  float multiplier = 100 / ( potential_->max() - potential_->min() ) ;
  for ( int z = 0 ; z < potential_->size()[2] ; z++ )
    for ( int y = 0 ; y < potential_->size()[1] ; y++ )
      for ( int x = 0 ; x < potential_->size()[0] ; x++ ){
	histogram[static_cast<int>( ( multiplier * (*potential_)(x,y,z) - potential_->min() ) + 0.5)]++ ;
	state_->setVoxel( x, y, z, far ) ;
	minimalActionMap_->setVoxel( x, y, z, Infinite ) ;
      }
  
  int lowHistogramSum = 0 ;
  float lowCut ;
  const int numberOfVoxels = potential_->size()[0]*potential_->size()[1]*potential_->size()[0] ;
  int i = 0 ;
  while( lowHistogramSum < numberOfVoxels / 2000 ){
    lowHistogramSum += histogram[i] ;
    //cout << "\tHistogram" << i<< " = " <<  histogram[i] <<"\tlowHistogramSum = " << lowHistogramSum << endl ;
    ++i ;
  }

  lowCut = ( i - 1 ) / multiplier + potential_->min( ) ;
  float newMultiplier = 1 / ( potential_->max( ) - lowCut ) ;
  for ( int z = 0 ; z < potential_->size()[2] ; z++ )
    for ( int y = 0 ; y < potential_->size()[1] ; y++ )
      for ( int x = 0 ; x < potential_->size()[0] ; x++ )
	if ( (*potential_)(x,y,z) < lowCut && (!lowerValueInaccessible_) )
	  potential_->setVoxel( x, y, z, 0. ) ;
	else 
	  potential_->setVoxel( x, y, z, newMultiplier * ( (*potential_)(x,y,z) - lowCut ) ) ;
  
//   i= 100 ;
//   while( highHistogramSum < numberOfVoxels / 10000 ){
//     highHistogramSum += histogram[i] ;
//     cout << "\tHistogram" << i<< " = " <<  histogram[i]<< "\thighHistogramSum = " << highHistogramSum << endl ;
//     --i ;
//   }
//   highCut = i+1 ;

//   if ( highCut != lowCut ) 
//     distanceStep_ *= ( highCut - lowCut ) * 0.01 ; 
//   else cout << "Narrow band Image !!!!!!!!!!!!" << endl ;
//   cout << "High Cut = " << highCut << "\tLowCut = " << lowCut << endl ;
    
  // Initial point's energy is 0
  minimalActionMap_->setVoxel( initialPoint_[0], initialPoint_[1], initialPoint_[2], 0. ) ;
  
  trialPoints_.clear( ) ;
  
  // Initial Point starts with "trial" state
  state_->setVoxel( initialPoint_, trial ) ;
  trialPoints_.insert( pair< float, Point3d > ( 0., initialPoint_ ) ) ;
  
   //cout << "Map Initialized" << endl ;
}

/*
 * computeMap()
 *
 * Computes the geodesic distance map to initial point via a fast marching approach 
 *
 */
void
GeodesicMinimalPath::computeMap( ){
  float epsilon = 0.0000001 ;
  //cout << "DistanceStep = " << distanceStep_ << endl ;
  //char c ; cin >> c ;
  float energy ;
  float trialEnergy ;
  
  //cout << "Lower Value Inaccesible : " <<  lowerValueInaccessible_ << endl ;
  
  Point3d neighbours[6], p ;
  int numberOfNeighbours ;

  multimap<float, Point3d, energyCompare>::iterator iteratorToBeErased( trialPoints_.begin( ) ) ;
  
  // Only for degugging use
  int unsuppressed = 0 ;

  // The map is computed when each voxel is "alive"
  // Computation is done via a "fast marching" method
  while( !trialPoints_.empty( ) ){
    // Trial point with lowest energy is set to "alive"
    //cout << "\tp = " << p ;
    p = trialPoints_.begin()->second ;
    //cout << "\tDealing with point p = " << p << " = " << trialPoints_.begin()->first << endl ;
    state_->setVoxel( p, alive ) ;
    trialPoints_.erase( trialPoints_.begin( ) ) ;
    
    // Energy will be computed in the neighbourhood of this point
    potential_->neighbours( p, neighbours, numberOfNeighbours ) ;
    
    for( int i = 0 ; i < numberOfNeighbours ; i++ ) {
      //cout << "\tNeighbour " << neighbours[i] << " = " << ( *minimalActionMap_ )( ( neighbours[i] ) ) 
      //	   << " : " << (*state_)(neighbours[i]) << endl ;
      // Don't treat alive points
      if( (*state_)(neighbours[i]) != alive) {
	if( lowerValueInaccessible_ && (*potential_)(neighbours[i]) == 1. )
	   energy = Infinite - 1 ;
	// Compute the new energy of the neighbour
	else
	   energy = solveEquation( neighbours[i] ) ;
	if ( energy == Infinite ) {
	  cout <<" INVALID MAP"  ;
	  return ;
	}
	// If neighbour is trial, ie in the propagating front, it must be erased from 
	// trial points list to be reinserted with a new energy value
	if( (*state_)(neighbours[i]) == trial ) { 
	  trialEnergy = ( *minimalActionMap_ )( ( neighbours[i] ) ) ;
	  // Find efficiently in the ordered list a point near and before the neighbour to erase
	  iteratorToBeErased = trialPoints_.lower_bound( trialEnergy - epsilon ) ;
	  
	  // Search the neighbour in the vincinity and erase it
	  while( iteratorToBeErased != trialPoints_.end( ) && iteratorToBeErased->second != neighbours[i] ) 
	    ++iteratorToBeErased ;
	  if ( iteratorToBeErased != trialPoints_.end( ) )
	    trialPoints_.erase( iteratorToBeErased ) ;
	  else 
	    // In this case, a bug has occured, but no danger !
	    ++unsuppressed ;
	}
	
	// Neighbour is now considered a trial point
	state_->setVoxel(neighbours[i], trial ) ;
	trialPoints_.insert( pair<float, Point3d>( energy, neighbours[i] ) ) ;
	
	//cout << "\tp has now energy = " << energy << endl << endl ;
	// With a new energy
	minimalActionMap_->setVoxel( neighbours[i], energy ) ;
      }
    }
    //char c ; cin >> c ;
  }
  //cout << endl ;
  mustRecompute_ = false ;
  
  AimsData<float> minimalActionMap( potential_->size()[0], potential_->size()[1], potential_->size()[2] ) ;
  AimsData<float> pot( potential_->size()[0], potential_->size()[1], potential_->size()[2] ) ;
  
  for ( int z = 0 ; z < potential_->size()[2] ; z++ )
    for ( int y = 0 ; y < potential_->size()[1] ; y++ )
      for ( int x = 0 ; x < potential_->size()[0] ; x++ ){
	minimalActionMap( x, y, z ) = (*minimalActionMap_)(x,y,z) ;
	 pot( x, y, z ) = (*potential_)(x,y,z) ;
      }
  Writer< AimsData< float> > wri( "minimalActonMap.ima") ;
  wri.write( minimalActionMap ) ;
  Writer< AimsData< float> > wri1( "potential.ima") ;
  wri1.write( pot ) ;
  
  //cout << endl << "Map Computed, unsuppressed trial points ( should be 0 ) = " << unsuppressed << endl ;
}



inline static void energyMin( double ux, double uy, double uz, double& umin )
{
  if( ux <= uy && ux <= uz )
    umin = ux ;
  else if( uy <= ux && uy <= uz)
    umin = uy ;
  else umin = uz ;
}

inline static void energyMax( double ux, double uy, double uz, double& umax )
{
  if( ux >= uy && ux >= uz )
    umax = ux ;
  else if( uy >= ux && uy >= uz)
    umax = uy ;
  else umax = uz ;
}

inline static void energyMedium( double ux, double uy, double uz, double& umedium )
{
  if( (ux >= uy && ux <= uz) || (ux <= uy && ux >= uz) )
    umedium = ux ;
  else if( (uy >= ux && uy <= uz) || (uy <= ux && uy >= uz) )
    umedium = uy ;
  else umedium = uz ;
}

inline static double
trinomeSolution( double a, double b, double c ){
  double b2 = b*b ;
  double det = b2 - 4*a*c ;
  if( det >= 0. ){
    return ( -b + sqrt( det ) ) / (2*a) ;
  }
  cout << "Det < 0 " << endl ;
  return Infinite ;
}

inline double
GeodesicMinimalPath::solveEquation( const Point3d& p ){
  
  // Equation to be solved is ||Grad(U)|| = Potential
  // U is unknown in some point of p's neighbour hood, sograd has to be approximated
  // Solve " ( max{ u - min{ U(k-1,l,m) , U(k+1,l,m)}, 0 } )^2 + 
  //           max{ u - min{ U(k,l-1,m) , U(k,l+1,m)}, 0 } )^2 + 
  //           max{ u - min{ U(k,l,m-1) , U(k,l,m+1)}, 0 } )^2  = Potential(k, l, m) "
  int x( p[0] ), y( p[1] ), z( p[2] ) ;
  
  double uxmin, uymin, uzmin ;

  // min{ U(k-1,l,m) , U(k+1,l,m)}
  uxmin = min( ( x > 0 ? (*minimalActionMap_)(x-1,y,z) : Infinite ), 
	       ( x < minimalActionMap_->size()[0]-1 ? (*minimalActionMap_)(x+1,y,z) : Infinite) ) ;

  // min{ U(k,l-1,m) , U(k,l+1,m)}
  uymin = min( ( y > 0 ? (*minimalActionMap_)(x,y-1,z) : Infinite ), 
	       ( y < minimalActionMap_->size()[1]-1 ? (*minimalActionMap_)(x,y+1,z) : Infinite) ) ;
  
  //min{ U(k,l,m-1) , U(k,l,m+1)}
  uzmin = min( ( z > 0 ? (*minimalActionMap_)(x,y,z-1) : Infinite ), 
	       ( z < minimalActionMap_->size()[2]-1 ? (*minimalActionMap_)(x,y,z+1) : Infinite) ) ;
  
  // Sort these minima.
  double umax, umedium, umin, u, p2 = ( (*potential_)( x, y, z ) + distanceStep_ ) * 
    ( (*potential_)( x, y, z ) + distanceStep_ ) ;
  energyMin( uxmin, uymin, uzmin, umin ) ;
  energyMedium( uxmin, uymin, uzmin, umedium ) ;
  energyMax( uxmin, uymin, uzmin, umax ) ;
  //cout 	<< "\tuxmin : " << uxmin << "\tuymin : " << uymin << "\tuzmin : " << uzmin << endl 
  //	<< "\tumin : " << umin << "\tumedium : " << umedium << "\tumax : " << umax << endl
  //	<< "\tpotential : " << (*potential_)( x, y, z ) << "\tdistanceStep : "<< distanceStep_ << "\tp2 : "<< p2 << endl ;
  
  // Case  u > umin && u < umedium
  if( p2 < ( umedium - umin ) * ( umedium - umin ) ){
    u = trinomeSolution( 1, -2*umin, umin*umin - p2 ) ;
    if( u > umin && u < umedium && u != Infinite ) 
      return u ;
  }
  // Case u > umedium && u < umax
  else if( p2 > ( umedium - umin ) * ( umedium - umin ) &&  
	   p2 < ( umax - umedium ) * ( umax - umedium ) + ( umax - umin ) * ( umax - umin ) ){
    u = trinomeSolution( 2, -2*( umin + umedium ), umin*umin + umedium*umedium - p2 ) ;
    if ( u > umedium && u < umax && u != Infinite )
      return u ;
  }
  // Case u > umax 
  else if( p2 > ( umax - umedium ) * ( umax - umedium ) + ( umax - umin ) * ( umax - umin ) ){
    u = trinomeSolution( 3, -2*( umin + umedium + umax ), umin*umin + umedium*umedium + umax*umax - p2 ) ;
    if( u > umax && u != Infinite )
      return u ;
  } 
  
  // No solution, a bug has occurred
  return Infinite ;
}


/*
 * getMinimalPath()
 *
 * Returns the minimal path from InitialPoint to FinalPoint 
 *
 */
list < Point3d > 
GeodesicMinimalPath::getMinimalPath( const Point3d& finalPoint )
{
  if ( mustRecompute_ ){
    initMap( ) ;
    computeMap( ) ;
  }
  return backtraceMinimalPath( finalPoint ) ;
}

/*
 * backtraceMinimalPath()
 *
 * Computes  minimal path from FinalPoint back to InitialPoint by a simple gradient descent
 *
 * Because it is a distance map, gradient descent always ends on InitialPoint
 *
 */
list < Point3d > 
GeodesicMinimalPath::backtraceMinimalPath( const Point3d& finalPoint )
{
  Point3d neighbours[6], p ;
  int numberOfNeighbours ;

  Point3d currentPoint = finalPoint ;
  list< Point3d > minimalPath ;
  minimalPath.push_back( currentPoint ) ;
  
  while( (*minimalActionMap_)( currentPoint ) > distanceStep_ / 2. ){	
    minimalActionMap_->neighbours( currentPoint, neighbours, numberOfNeighbours ) ;
    
    for ( int i = 0 ; i < numberOfNeighbours ; i++ ){
      //cout << "\tNeighbour " << neighbours[i] << "  =  " <<  (*minimalActionMap_)( neighbours[i] ) << endl ;
      if ( (*minimalActionMap_)( neighbours[i] ) < (*minimalActionMap_)( currentPoint ) )
	currentPoint = neighbours[i] ;
    }
    //cout << endl << "CurrentPoint : " << currentPoint << "  =  " << (*minimalActionMap_)( currentPoint ) << endl ;
    minimalPath.push_back( currentPoint ) ;
    //char c ; cin >> c ;
  }
  
  return minimalPath ;
}

template< typename T > 
inline static T& min( T& u, T& v ){
  return (u > v ? v : u ) ;	
}
