
#ifndef DISTANCE_D_H
#define DISTANCE_D_H

#include <cstdlib>
#include <aims/classification/distance.h>
#include <math.h>


template <class T>
aims::Distance<T>::Distance()
{
}

template <class T>
aims::Distance<T>::~Distance()
{
}

template <class T>
float aims::Distance<T>::norm1( const std::vector<T>& ind1, const std::vector<T>& ind2, 
				unsigned int beginIndex, unsigned int endIndex )
{
  unsigned int size1, size2 ;
  size1 = ind1.size() ;
  size2 = ind2.size() ;
  ASSERT( size1 == size2 ) ;

  float dist = 0. ;  
  for( unsigned int i = beginIndex ; i <= endIndex ; ++i )
    dist += fabs( double( ind1[i] - ind2[i] ) ) ;   
  return dist ;
}


template <class T>
float aims::Distance<T>::norm2( const std::vector<T>& ind1, const std::vector<T>& ind2, 
				unsigned int beginIndex, unsigned int endIndex )
{
  unsigned int size1, size2 ;
  size1 = ind1.size() ;
  size2 = ind2.size() ;
  ASSERT( size1 == size2 ) ;

  float dist = 0. ;
  for( unsigned int i = beginIndex ; i <= endIndex ; ++i )
    dist += ( ind1[i] - ind2[i] ) * ( ind1[i] - ind2[i] ) ;
  return sqrt( dist ) ;
}


template <class T>
float aims::Distance<T>::norm2sqr( const std::vector<T>& ind1, const std::vector<T>& ind2, 
				   unsigned int beginIndex, unsigned int endIndex )
{    
  unsigned int size1, size2 ;
  size1 = ind1.size() ;
  size2 = ind2.size() ;
  ASSERT( size1 == size2 ) ;

  float dist = 0. ;
  for( unsigned int i = beginIndex ; i <= endIndex ; ++i )
    dist += ( ind1[i] - ind2[i] ) * ( ind1[i] - ind2[i] ) ;   
  return dist ;
}


template <class T>
float aims::Distance<T>::infiniteNorm( const std::vector<T>& ind1, const std::vector<T>& ind2, 
				       unsigned int beginIndex, unsigned int endIndex )
{
  unsigned int size1, size2 ;
  size1 = ind1.size() ;
  size2 = ind2.size() ;
  ASSERT( size1 == size2 ) ;

  float dist = 0., distmax = 0. ;
  for( unsigned int i = beginIndex ; i <= endIndex ; ++i ){
    dist = fabs( double( ind1[i] - ind2[i] ) ) ;   
    if( dist > distmax ) distmax = dist ;
  }
  return distmax ;
}

#endif
