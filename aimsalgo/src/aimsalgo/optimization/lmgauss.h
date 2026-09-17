
#ifndef AIMS_OPTIMIZATION_LMGAUSS_H
#define AIMS_OPTIMIZATION_LMGAUSS_H

#include <aims/optimization/lmfunc.h>


template < class T >
class LMGaussian : public LMFunction< T >
{
public:

  LMGaussian( T k=(T)1.0, T m=(T)0.0, T s=(T)1.0 );

  T apply( T );
  T eval( T );
};


template< class T > inline
LMGaussian< T >::LMGaussian( T k, T m, T s ) : LMFunction< T >()
{
  this->par.push_back( k );
  this->par.push_back( m );
  this->par.push_back( s );

  this->der = std::vector< T >( 3 );
}



template< class T > inline
T LMGaussian< T >::eval( T x )
{
  T arg = ( x - this->par[ 1 ] ) / this->par[ 2 ];
  T ex = (T)exp( -1.0 * (arg * arg ) );
  T fac = (T)2 * this->par[ 0 ] * ex * arg;

  this->der[ 0 ] = ex;
  this->der[ 1 ] = fac / this->par[ 2 ];
  this->der[ 2 ] = fac * arg / this->par[ 2 ];

  return this->par[ 0 ] * ex;
}


template< class T > inline
T LMGaussian< T >::apply( T x )
{
  T arg = ( x - this->par[ 1 ] ) / this->par[ 2 ];

  return this->par[ 0 ] * (T)exp( -1.0 * ( arg * arg ) );
}

#endif
