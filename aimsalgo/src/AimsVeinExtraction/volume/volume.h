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


#ifndef AIMS_VOLUME_VOLUME_H
#define AIMS_VOLUME_VOLUME_H

#include <aims/vector/vector.h>
#include <list>

template <typename T>
class Volume {
public:
  Volume( const Point3d& volumeSize, T* data = 0, const Point3df& voxelSize = Point3df( 1., 1., 1.) ) ; 
  Volume( const Volume<T>& v ) ;
  ~Volume( ) 
    {
      if( data_ )
	delete [] data_ ;
    }
  const T& operator()(int i, int j, int k ) const ;
  const T& operator() (const Point3d& p ) const ;
  
  void setVoxel( int x, int y, int z, const T& value ) {
    if(!data_)
      return ;
    data_[z*size_[1]*size_[0] + y*size_[0] + x] = value ;
    changed_ = true ;
  }
  
  void setVoxel( const Point3d& p, const T& value ) {
    if(!data_)
      return ;
    data_[ p[2]*size_[1]*size_[0] + p[1]*size_[0] + p[0] ] = value ;
    changed_ = true ;
  }
  
  //T& operator()(int x, int y, int z ) ;
  //T& operator() (const Point3d& p ) ;
  const Point3d& size() const { return size_ ; }
  
  const Point3df& voxelSize() const { return voxelSize_ ; }
  
  void neighbours( const Point3d& p, Point3d result[6], int& numberOfNeighbours ) const ;
  
  float interpolation( const Point3df& p ) ;
  
  float interpolation( float x, float y, float z ) ;
  
  const T& min( ) { 
    if(!data_)
      min_ = 0 ;
    else if ( changed_ ){
      std::cout << "min has changed" << std::endl ;
      max_ = min_ = *data_ ;
      T* data = data_ ; 
      for ( int z = 0 ; z < size_[2] ; z++ )
	for ( int y = 0 ; y < size_[1] ; y++ )
	  for ( int x = 0 ; x < size_[0] ; x++, data++ )
	    if( (*data) < min_ )
	      min_ = (*data) ;
	    else if(  (*data) > max_ )
	      max_ = (*data) ;
      changed_ = false ;
    }
    return min_ ; 
  }

  const T& max( ) { 
    if( !data_ )
      max_ = 0 ;
    else if ( changed_ ){
      std::cout << "max has changed" << std::endl ;
      max_ = min_ = *data_ ;
      T* data = data_ ; 
      for ( int z = 0 ; z < size_[2] ; z++ )
	for ( int y = 0 ; y < size_[1] ; y++ )
	  for ( int x = 0 ; x < size_[0] ; x++, data++ )
	    if( (*data) < min_ )
	      min_ = (*data) ;
	    else if(  *data > max_ )
	      max_ = (*data) ;
      changed_ = false ;
    }
    return max_ ; 
  }

  void normalize( ) ;
private:
  bool changed_ ;
  T * data_ ;
  Point3d size_ ;
  Point3df voxelSize_ ;
  T min_, max_ ;
} ;

template < typename T>
Volume<T>::Volume( const Point3d& size, T* data, const Point3df& voxelSize ) :
	size_(size), voxelSize_(voxelSize)
{
  data_ = new T[size_[0]*size_[1]*size_[2]] ;
  T* myData = data_ ;
  if ( data )
    for( int z = 0 ; z < size_[2] ; z++ )
      for( int y = 0 ; y < size_[1] ; y++ )
	for( int x = 0 ; x < size_[0] ; x++, data++, myData++ ){
	  *myData = *data ;
	  if( *myData < min_ )
	    min_ = *myData ;
	  else if(  *myData > max_ )
	    max_ = *myData ;
	}
  changed_ = false ;
}

template < typename T>
Volume<T>::Volume( const Volume<T>& v ) :
  size_( v.size() ), voxelSize_(v.voxelSize())
{
  data_ = new T[size_[0]*size_[1]*size_[2]] ;
  T* data = data_ ;
  for( int z = 0 ; z < size_[2] ; z++ )
    for( int y = 0 ; y < size_[1] ; y++ )
      for( int x = 0 ; x < size_[0] ; x++, data++ ){
	*data = v( x,y,z ) ;
	if( *data < min_ )
	  min_ = *data ;
	else if(  *data > max_ )
	  max_ = *data ;
      }
  changed_ = false ;
}


template < typename T>
inline const T&
Volume<T>::operator()(int x, int y, int z ) const {
  return data_[ z*size_[1]*size_[0] + y*size_[0] + x ] ;
}


template < typename T>
inline void
Volume<T>::neighbours( const Point3d& p, Point3d result[6], int& numberOfNeighbours ) const
{  
  numberOfNeighbours = 0 ;
  //std::cout << " Is the point " << p << " out of Image " << std::endl ;

  //std::cout << " Size : " << size_ ;
  if( p[0] > 0 )
    result[numberOfNeighbours++] = p + Point3d( -1, 0, 0 ) ;	
  
  if( p[0] < size_[0]-1 )
    result[numberOfNeighbours++] = p + Point3d( 1, 0, 0 ) ;	
  
  if( p[1] > 0 )
    result[numberOfNeighbours++] = p + Point3d( 0, -1, 0 ) ;	
  
  if( p[1] < size_[1]-1 )
    result[numberOfNeighbours++] = p + Point3d( 0, 1, 0 ) ;	
  
  if( p[2] > 0 )
    result[numberOfNeighbours++] = p + Point3d( 0, 0, -1 ) ;	
  
  if( p[2] < size_[2]-1 )
    result[numberOfNeighbours++] = p + Point3d( 0, 0, 1 ) ;	
  
  //std::cout << "numberOfNeighbours =  " << numberOfNeighbours << std::endl ;

}

template < typename T >
inline const T& 
Volume<T>::operator() (const Point3d& p ) const {
  return data_[ p[2]*size_[1]*size_[0] + p[1]*size_[0] + p[0] ] ;
}

template < typename T >
void 
Volume<T>::normalize( )
{
  T * data = data_ ;
  changed_ = true ;
  T minimum = min(), maximum = max() ;
  std::cout << "\t max : " << maximum << "\t min : " << minimum << std::endl ;
  T manmin = *data, manmax = *data ;
  bool flatVolume = ( minimum == maximum ) ;
  if(!flatVolume)
    for ( int z = 0 ; z < size()[2] ; z++ )
      for ( int y = 0 ; y < size()[1] ; y++ ) 
	for ( int x = 0 ; x < size()[0] ; x++, data++ ){
	   if ( manmin > *data ) manmin = *data ;
	   if ( manmax < *data ) manmax = *data ;
	  *data = ( flatVolume ? *data - minimum : ( *data - minimum ) / ( maximum - minimum ) ) ;
	}
  if( manmin != min_ || manmax != max_ )
    std::cout << "\n\nBad Calcul\n\nmin : "<< manmin << "\tmax : "<< manmax ;
  min_ = 0 ;
  max_ = 1 ;
  changed_ = false ;
}

template < typename T >
float 
Volume<T>::interpolation( const Point3df& p )
{
  if ( p[0] < 0. || p[1] < 0. || p[2] < 0. || p[0] > size_[0] - 1 || p[1] > size_[1] - 1 || p[2] > size_[2] - 1 ){
    //std::cout << "\nOut of image" << std::endl ;
    return 0. ;
  }
  float r, s, t, rm, sm, tm, w[8] ;
  Point3d pint( static_cast<int>( p[0]), static_cast<int>( p[1]), static_cast<int>( p[2] ) ) ;
  r = p[0] - pint[0] ;
  s = p[1] - pint[1] ;
  t = p[2] - pint[2] ;

  rm = 1. - r;
  sm = 1. - s;
  tm = 1. - t;

  w[0] = rm * sm * tm;
  w[1] = r * sm * tm;
  w[2] = rm * s * tm;
  w[3] = r * s * tm;
  w[4] = rm * sm * t;
  w[5] = r * sm * t;
  w[6] = rm * s * t;
  w[7] = r * s * t;

  return (*this)( pint ) * rm * sm * tm 
    + (*this)( pint[0]+1, pint[1], pint[2] ) * r * sm * tm 
    + (*this)( pint[0], pint[1]+1, pint[2] ) * rm * s * tm
    + (*this)( pint[0]+1, pint[1]+1, pint[2] ) * r * s * tm
    + (*this)( pint[0], pint[1], pint[2]+1 ) * rm * sm * t
    + (*this)( pint[0]+1, pint[1], pint[2]+1 ) * r * sm * t
    + (*this)( pint[0], pint[1]+1, pint[2]+1 ) * rm * s * t
    + (*this)( pint[0]+1, pint[1]+1, pint[2]+1 ) * r * s * t ;
}

template < typename T >
float 
Volume<T>::interpolation( float x, float y, float z )
{
  
  if ( x < 0. || y < 0. || z < 0. || x > size_[0] - 1 || y > size_[1] - 1 || z > size_[2] - 1 ){
    //std::cout << "\nOut of image" << std::endl ;
    return 0. ;
  }

  float r, s, t, rm, sm, tm, w[8] ;
  Point3df p(x, y, z) ;
  Point3d pint( static_cast<int>( p[0]), static_cast<int>( p[1]), static_cast<int>( p[2] ) ) ;
  r = p[0] - pint[0] ;
  s = p[1] - pint[1] ;
  t = p[2] - pint[2] ;

  rm = 1. - r;
  sm = 1. - s;
  tm = 1. - t;

  w[0] = rm * sm * tm;
  w[1] = r * sm * tm;
  w[2] = rm * s * tm;
  w[3] = r * s * tm;
  w[4] = rm * sm * t;
  w[5] = r * sm * t;
  w[6] = rm * s * t;
  w[7] = r * s * t;

  return (*this)( pint ) * rm * sm * tm 
    + (*this)( pint[0]+1, pint[1], pint[2] ) * r * sm * tm 
    + (*this)( pint[0], pint[1]+1, pint[2] ) * rm * s * tm
    + (*this)( pint[0]+1, pint[1]+1, pint[2] ) * r * s * tm
    + (*this)( pint[0], pint[1], pint[2]+1 ) * rm * sm * t
    + (*this)( pint[0]+1, pint[1], pint[2]+1 ) * r * sm * t
    + (*this)( pint[0], pint[1]+1, pint[2]+1 ) * rm * s * t
    + (*this)( pint[0]+1, pint[1]+1, pint[2]+1 ) * r * s * t ;
}

// template < typename T >
// T& 
// Volume<T>::operator() (const Point3d& p ) {
//   return data_[ p[2]*size_[1]*size_[2] + p[1]*size_[1] + p[0] ] ;
  //}


#endif
