
#ifndef AIMS_OPTIMIZATION_LMGAMMA_H
#define AIMS_OPTIMIZATION_LMGAMMA_H

#include <aims/optimization/lmfunc.h>


// Gamma-variate function:
// f(t) = k * ( t - ta ) ^ a * exp( - ( t - ta ) / b )   si t > ta
// f(t) = 0   sinon
template < class T >
class LMGamma : public LMFunction< T >
{
public:

  LMGamma( T k=(T)1.0, T ta=(T)4.0, T a=(T)3.0, T b=(T)1.5 );

  T apply( T );
  T eval( T );
};


template< class T > inline
LMGamma< T >::LMGamma( T k, T ta, T a, T b ) : LMFunction< T >()
{
  this->par.push_back( k );
  this->par.push_back( ta );
  this->par.push_back( a );
  this->par.push_back( b );

  this->der = std::vector< T >( 4 );
}


template< class T > inline
T LMGamma< T >::eval( T x )
{
  if ( x > this->par[ 1 ] )
    {
      // k 
      double k = (double)this->par[ 0 ];
      // a
      double a = (double)this->par[ 2 ];
      // t - ta
      double tta = (double)( x - this->par[ 1 ] );
      // ln( t - ta )
      double ltta = log( tta );
      // ( t - ta ) / b
      double ttab = tta / this->par[ 3 ];
      // ( t - ta ) ^ a * exp( - ( t - ta ) / b )
      // = exp( a * ln( t - ta ) - ( t - ta ) / b )
      double expf = exp( a * ltta - ttab );

      // f
      double f = k * expf;

      // df / dk
      this->der[ 0 ] = (T)expf;

      // df / dta
      this->der[ 1 ] = (T)( k * ( ttab - a ) * exp( ( a - 1 ) * ltta - ttab ) );

      // df / da
      this->der[ 2 ] = (T)( k * ltta * expf );

      // df / db
      double b2 = (double)( this->par[ 3 ] * this->par[ 3 ] );
      this->der[ 3 ] = (T)( k * exp( ( a + 1 ) * ltta - ttab ) / b2 );

      return (T)f;
    }
  else
    {
      this->der[ 0 ] = this->der[ 1 ] = this->der[ 2 ] = this->der[ 3 ] = (T)0;

      return (T)0;
    }
}


template< class T > inline
T LMGamma< T >::apply( T x )
{
  if ( x > this->par[ 1 ] )
    {
      // t - ta
      double tta = (double)( x - this->par[ 1 ] );
      // ln( t - ta )
      double ltta = log( tta );
      // ( t - ta ) / b
      double ttab = tta / this->par[ 3 ];

      return (T)( this->par[ 0 ] * exp( this->par[ 2 ] * ltta - ttab ) );
    }
  else return (T)0;
}

#endif
