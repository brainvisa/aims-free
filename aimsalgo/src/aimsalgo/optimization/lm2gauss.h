
#ifndef AIMS_OPTIMIZATION_LM2GAUSS_H
#define AIMS_OPTIMIZATION_LM2GAUSS_H

#include <aims/optimization/lmfunc.h>


template < class T >
class LM2Gaussian : public LMFunction< T >
{
public:

  LM2Gaussian( T k1=(T)1.0, T m1=(T)0.0, T s1=(T)1.0, 
	       T k2=(T)1.0, T m2=(T)0.0, T s2=(T)1.0 );

  T apply( T );
  T eval( T );
};


template< class T > inline
LM2Gaussian< T >::LM2Gaussian( T k1, T m1, T s1, T k2, T m2, T s2 ) 
  : LMFunction< T >()
{
  this->par.push_back( k1 );
  this->par.push_back( m1 );
  this->par.push_back( s1 );
  this->par.push_back( k2 );
  this->par.push_back( m2 );
  this->par.push_back( s2 );

  this->der = std::vector< T >( 6 );
}



template< class T > inline
T LM2Gaussian< T >::eval( T x )
{
  T y = (T)0;

  for ( int i=0; i<6; i+=3 ) 
    {
      T arg = ( x - this->par[ i + 1 ] ) / this->par[ i + 2 ];
      T ex = (T)exp( -1.0 * ( arg * arg ) );
      T fac = (T)2 * this->par[ i ] * ex * arg;

      y += this->par[ i ] * ex;

      this->der[ i ] = ex;
      this->der[ i + 1 ] = fac / this->par[ i + 2 ];
      this->der[ i + 2 ] = fac * arg / this->par[ i + 2 ];
    }

  return y;
}


template< class T > inline
T LM2Gaussian< T >::apply( T x )
{
  T y = (T)0;

  for ( int i=0; i<6; i+=3 ) 
    {
      T arg = ( x - this->par[ i + 1 ] ) / this->par[ i + 2 ];

      y += this->par[ i ] * (T)exp( -1.0 * ( arg * arg ) );
    }

  return y;
}

#endif
