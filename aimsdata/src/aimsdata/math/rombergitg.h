/* Copyright (c) 1995-2005 CEA
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

/*
 *  Romberg's numerical integration.
 */
#ifndef AIMS_MATH_ROMBERGITG_H
#define AIMS_MATH_ROMBERGITG_H


#include <cstdlib>
#include <aims/math/trapezeitg.h>
#include <aims/data/data.h>
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
  AimsData< REAL > s( _jmax ), h( _jmax + 1 );

  if ( a == b )
    return 0.0;
  
  h( 0 ) = 1.0;
  for ( int j = 0; j < _jmax; j++ )
  {
    TrapezeIntegratorOf<REAL> integrator;
    s( j ) = integrator.stage( func, a, b, j + 1 );
    if ( j + 1 >= _k )
    {
      AimsData< REAL > tmph( _k ), tmps( _k );
      for ( int kk = 0; kk < _k; kk++ )
      {
        tmph( kk ) = h( j - _k + kk + 1 );
        tmps( kk ) = s( j - _k + kk + 1 );
      }
      ss = AimsLagrangeInterpolationOf<REAL>( tmph, tmps, 0.0, &dss );
      if ( fabs( dss ) <= _eps * fabs( ss ) )
        return ss;
    }
    h( j + 1 ) = 0.25 * h( j );
  }
  return 0.0;
}


// For backward compatibility
typedef RombergIntegratorOf<float> RombergIntegrator;

} // namespace aims

#endif
