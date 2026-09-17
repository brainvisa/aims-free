/*
 *  Trapeze numerical integration.
 */
#ifndef AIMS_MATH_TRAPEZEITG_H
#define AIMS_MATH_TRAPEZEITG_H


#include <cstdlib>
#include <aims/math/integrator.h>
#include <math.h>

namespace aims {

template <class REAL>
class TrapezeIntegratorOf
{
  public:

    // to have 2 ^ nStep + 1 steps 
    TrapezeIntegratorOf( int nStep = 10 ) : _nStep( nStep ) { }
    virtual ~TrapezeIntegratorOf() { }

    virtual REAL stage( const Integrable& func,
                        REAL a, REAL b, int n ) const;
    virtual REAL eval( const Integrable& func, REAL a, REAL b ) const;

  private:

    int _nStep;
};


template <class REAL> inline 
REAL TrapezeIntegratorOf<REAL>::stage( const Integrable& func,
                                       REAL a, REAL b, int n ) const
{
  REAL  x, tnm, sum, del;

  // s was not initialized and is used if n != 1. I choosed to initialize it
  // to 0 without thinking too much to the consequences.
  REAL s = 0;

  int it = 1, j = 1;

  if ( n == 1 )
    s = 0.5 * (b - a) * ( func.valueAt( a ) + func.valueAt( b ) );
  else
  {
    for ( it = 1, j = 1; j < n - 1; j++ )
      it <<= 1;
    tnm = it;
    del = ( b - a ) / tnm;
    x = a + 0.5 * del;
    for ( sum = 0.0, j = 1; j <= it; j++, x += del )
      sum += func.valueAt( x );
    s = 0.5 * ( s + ( b - a ) * sum / tnm );
  }
  return s;
}

template <class REAL> inline 
REAL TrapezeIntegratorOf<REAL>::eval( const Integrable& func,
                                      REAL a, REAL b ) const
{
  REAL olds = -1.0e30;
  REAL EPS = 1.0e-5;
  REAL s;

  for ( int j = 1; j <= _nStep + 1; j++ ) {
    s = stage( func, a, b, j );
    if ( j > 5 )
      if ( fabs( s - olds ) < EPS * fabs( olds ) ||
           ( s == 0.0 && olds == 0.0 ) )
        return s;
    olds = s;
  }
    
  return 0.0;
}


// For backward compatibility
typedef TrapezeIntegratorOf<float> TrapezeIntegrator;

} // namespace aims

#endif
