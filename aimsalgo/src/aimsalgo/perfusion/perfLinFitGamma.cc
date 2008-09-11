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



#include <aims/perfusion/perfLinFitGamma.h>
#include <aims/optimization/lmgamma.h>
#include <math.h>

using namespace aims;
using namespace std;


AimsData< float > 
PerfusionLinearFitGammaVariate::doit( AimsData< float >& d, 
				      BucketMap< Void >& mask, int skip,
				      map< Point3d, LMGamma< float >, 
				      bestPoint3d >& mFit )
{
  AimsData< float > res( d.dimX(), d.dimY(), d.dimZ(), d.dimT() );
  res.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ(), d.sizeT() );

  BucketMap<Void>::Bucket::iterator it = mask[0].begin();

  int t, tm=0, dt = d.dimT();
  float st = d.sizeT();
  AimsData< float > ytmp( dt );
  ytmp.setSizeX( st );

  int t0 = 0, t1 = 0, t2 = 0;
  float fm = 0.0f, fm2 = 0.0f, fm10 = 0.0f;
  float Sx1=0.0f, Sx2=0.0f, Sy=0.0f, Sx1y=0.0f, Sx2y=0.0f, Sx12=0.0f;
  float Sx22=0.0f, Sx1x2=0.0f, A11=0.0f, A12=0.0f, A22=0.0f, A1y=0.0f;
  float A2y=0.0f, D=0.0f, a1=0.0f, a2=0.0f, mu=0.0f;
  float x1 = 0.0f, x2 = 0.0f, y = 0.0f;
  float t0f = 0.0f, k = 0.0f, beta = 0.0f;
  float yi = 0.0f, w = 0.0f, Sw = 0.0f;

  LMGamma< float > lmg;

  while( it != mask[0].end() )
    {
      Point3d pt = it->first;

      for ( t=0; t<dt; t++ )  ytmp( t ) = d( pt[0], pt[1], pt[2], t );
      
      // Look for the maximum
      for ( fm=0.0f, t=skip; t<dt; t++ )
        if ( ytmp( t ) > fm )
	  {
	    fm = ytmp( t );
	    tm = t;
	  }

      // Look for left half maximum appearence time
      t = tm;
      fm2 = fm / 2.0f;
      while ( ytmp( t ) > fm2 && t > skip )  t--;
      t1 = t;
      
      // Look for an estimation of t0 (take-off time)
      fm10 = fm / 10.0f;
      while ( ytmp( t ) > fm10 && t > skip )  t--;
      t0 = t;
      float y1 = ytmp( t0 );
      float y2 = ytmp( t1 );

      // Look for t=3*tm/4
      t = tm;
      float fm3 = 3.0f * fm / 4.0f;
      while ( ytmp( t ) > fm3 && t < dt )  t++;
      int t3 = t;

      // Look for the right half maximum appearence time
      while ( ytmp( t ) > fm2 && t <dt )  t++;
      t2 = t;

      int tlimite = dt;
      if ( t3 < dt ) tlimite = t3;
      else t3 = tm;

      if ( t2 < dt ) tlimite = t2;
      else t2 = t3;

      if ( y2 > y1 && tm > t1 && t2 > tm && ( t2 - t1 ) >= 3 )
        {
          // Linear extrapolation for t0 refinement
          t0f = ( y2 * (float)t0 - y1 * (float)t1 ) / ( y2 - y1 );
	  if ( t0f >= (float)t0 )  t0f = (float)t0 - 0.01;
 
          // Perform the fit
          Sx1 = Sx2 = Sy = Sx1y = Sx2y = Sx12 = Sx22 = Sx1x2 = Sw = 0.0f;
          for ( t=t0; t<tlimite; t++ )
            {
	      x2 = (float)t - t0f;
	      x1 = (float)log( x2 );
	      yi = ytmp( t );
	      w = yi * yi;
	      y = (float)log( yi + 0.01 );
	      Sw += w;
	      Sx1 += w * x1;
	      Sx2 += w * x2;
	      Sy += w * y;
	      Sx1y += w * x1 * y;
	      Sx2y += w * x2 * y;
	      Sx12 += w * x1 * x1;
	      Sx22 += w * x2 * x2;
	      Sx1x2 += w * x1 * x2;
       	    }
	
          A11 = Sx12 - Sx1 * Sx1 / Sw;
          A12 = Sx1x2 - Sx1 * Sx2 / Sw;
          A22 = Sx22 - Sx2 * Sx2 / Sw;
          A1y = Sx1y - Sx1 * Sy / Sw;
          A2y = Sx2y - Sx2 * Sy / Sw;
      
          D = A11 * A22 - A12 * A12;
          a1 = ( A22 * A1y - A12 * A2y ) / D;
          a2 = ( A11 * A2y - A12 * A1y ) / D;
          mu = ( Sy - a1 * Sx1 - a2 * Sx2 ) / Sw;
      
          // alpha = a1
          k = (float)exp( mu );
          beta = -1.0f / a2;

          if ( a1 <= 0.0f || beta <= 0.0f || beta > 10.0f )
	    lmg = LMGamma< float >( 0.0f, 0.0f, 1.0f, 1.0f );
	  else 
            lmg = LMGamma< float >( k, t0f, a1, beta );
        }
      else lmg = LMGamma< float >( 0.0f, 0.0f, 1.0f, 1.0f );

      mFit[ pt ] = lmg;

      for ( t=0; t<dt; t++ )  
        res( pt[0], pt[1], pt[2], t ) = lmg.apply( (float)t );

      ++it;
    }

  return res;
}
