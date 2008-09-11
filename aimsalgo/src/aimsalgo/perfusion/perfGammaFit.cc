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


#include <aims/perfusion/perfGammaFit.h>
#include <math.h>

using namespace aims;

LMGamma< float > PerfusionGammaFit::doit( AimsData< float >& d, int skip )
{
  int t, tm = 0, dt = d.dimX();
  float fm = 0.0f, st = d.sizeT();

  // Look for the maximum
  for ( t=skip; t<dt; t++ )
    if ( d( t ) > fm )
      {
	fm = d( t );
	tm = t;
      }

  // Look for left half maximum appearence time
  int i = tm;
  float fm2 = fm / 2.0f;
  while ( d( i ) > fm2 && i > skip )  i--;
  int t1 = i;

  // Look for an estimation of t0 (take-off time)
  float fm10 = fm / 10.0f;
  while ( d( i ) > fm10 && i > skip )  i--;
  int t0 = i;

  // Look for t=3*tm/4
  i = tm;
  float fm3 = 3.0f * fm / 4.0f;
  while ( d( i ) > fm3 && i < dt )  i++;
  int t3 = i;

  // Look for right half maximum appearence time
  while ( d( i ) > fm2 && i < dt )  i++;
  int t2 = i;

  float a = 1.0f;
  int tlimite = dt;
  if ( t3 < dt )
    {
      tlimite = t3;
      a = 1.5f;
    }
  else t3 = tm;

  if ( t2 < dt )
    {
      tlimite = t2;
      a = 2.4f;
    }
  else t2 = t3;

  // Perform the fit
  if ( tm > t1 && t2 > tm && ( t2 - t1 ) >= 3 )
    {
      // Estimate the four parameters from the previous measures
      float deltaT = (float)( t2 - t1 ) * st;
      float tm_t0 = (float)( tm - t0 ) * st;
      float alpha = ( a * tm_t0 / deltaT ) * ( a * tm_t0 / deltaT );
      float beta = tm_t0 / alpha;
      float k = fm * exp( alpha * ( 1.0f - log( tm_t0 ) ) );

      LMGamma< float > lmg( k, (float)t0, alpha, beta );
      LevenbergMarquardt< float > levm( &lmg );

      AimsData< float > x( tlimite );
      AimsData< float > y( tlimite );
      for ( t=0; t<tlimite; t++ )
	{
	  x( t ) = (float)t * st;
	  y( t ) = d( t );
	}

      LMGamma< float > *theOut = (LMGamma< float > *)levm.doit( x, y );
      if ( !theOut  ) lmg = LMGamma< float >( 0.0f, 0.0f, 1.0f, 1.0f );

      for ( t=0; t<dt; t++ )  d( t ) = lmg.apply( (float)t * st );

      return lmg;
    }
  else 
    {
      for ( t=0; t<dt; t++ )  d( t ) = 0.0f;

      return LMGamma< float >( 0.0f, 0.0f, 1.0f, 1.0f );
    }
}
