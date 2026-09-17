/*
 *  Romberg's numerical integration.
 */
#ifndef AIMS_MATH_ROMBERGITG_H
#define AIMS_MATH_ROMBERGITG_H


#include <cstdlib>
#include <aims/math/trapezeitg.h>
#include <aims/math/lagrange.h>
#include <math.h>


namespace aims
{

template <class REAL>
class RombergIntegratorOf
{
  public:

    RombergIntegratorOf( const REAL &eps, int jmax, int k ) : _eps( eps ),
                                                        _jmax( jmax ),
                                                        _k( k ) { }
    virtual ~RombergIntegratorOf() { }

    virtual REAL eval( const Integrable& func, REAL a, REAL b ) const;

  private:

    REAL _eps;
    int _jmax;
    int _k;
};

template <class REAL>
REAL RombergIntegratorOf<REAL>::eval( const Integrable& func, 
                                       REAL a, REAL b ) const
{
  REAL ss, dss;
  std::vector< REAL > s( _jmax ), h( _jmax + 1 );

  if( a == b )
    return 0.0;
  
  h[ 0 ] = 1.0;
  for( int j = 0; j < _jmax; j++ )
  {
    TrapezeIntegratorOf<REAL> integrator;
    s[ j ] = integrator.stage( func, a, b, j + 1 );
    if( j + 1 >= _k )
    {
      std::vector< REAL > tmph( _k ), tmps( _k );
      for( int kk = 0; kk < _k; kk++ )
      {
        tmph[ kk ] = h[ j - _k + kk + 1 ];
        tmps[ kk ] = s[ j - _k + kk + 1 ];
      }
      ss = AimsLagrangeInterpolation( tmph, tmps, REAL(0.0), &dss );
      if( fabs( dss ) <= _eps * fabs( ss ) )
        return ss;
    }
    h[ j + 1 ] = 0.25 * h[ j ];
  }
  return 0.0;
}


// For backward compatibility
typedef RombergIntegratorOf<float> RombergIntegrator;

} // namespace aims

#endif
