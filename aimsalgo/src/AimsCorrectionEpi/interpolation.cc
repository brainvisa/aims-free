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



#include <cstdlib>
#include "interpolation.h"
#include <aims/vector/vector.h>
#include <aims/math/mathelem.h>
#include <iostream>
#include <math.h>
#include <assert.h>

using namespace aims;
using namespace std;


void linearInterpolation( const short* orig,
                          short* dest,
                          int size,
                          float res,
                          const AimsVector< float, 3 >& param )
{
  int i, j, yy;
  float lim1 = size - 1.0f;
  float lim2 = size - 0.5f;

  float scale = param[ 0 ];
  float T0 = param[ 1 ];
  float T1 = param[ 2 ];

  float y_base = 0.5 * ( size - 1 ) * ( 1.0f - scale + T1 ) + T0 / res;
  float y;

  short* pDest = dest;
  const short* pOrig = 0;
  for ( j = 0; j < size; j++ )
  {

    y = y_base;

    for ( i = 0; i < size; i++ )
    {

      if ( y >= -0.5f && y <= 0.0f )
        *pDest = orig[ i ];
      else if ( y >= lim1 && y <= lim2 )
        *pDest = orig[ ( size - 1 ) * size + i ];
      else if ( y < -0.5f || y > lim2 )
        *pDest = 0;
      else
      {

        yy = ( int )y;
        pOrig = &orig[ yy * size + i ];
        *pDest = (short)( *pOrig + ( y - yy ) * ( *( pOrig+size ) - *pOrig ) );

      }
      y -= T1;
      pDest ++;

    }
    y_base += scale;

  }


}


static 
float betaSpline3( float u )
{

  float valabs = fabs( u );

  if ( valabs <= 1 )
    return 0.5 * cub( valabs ) - sqr( valabs ) + 2.0 / 3.0;
  else if ( valabs > 1 && valabs <= 2 )
    return cub( 2.0 - valabs ) / 6.0;

  return 0.0;

}


static 
void toeplitz( const float* r,
               const float* y,
               float* x, int size )
{


  int j, k, m, m1, m2;
  float sxn, sd, sgn, shn, sgd, pp, qq, pt1, pt2, qt1, qt2;

  float* g = new float[ size ];
  float* h = new float[ size ];

  x[ 0 ] = y[ 0 ] / r[ size - 1 ];
  g[ 0 ] = r[ size - 2 ] / r[ size - 1 ];
  h[ 0 ] = r[ size ] / r[ size - 1 ];

  for ( m = 1; m <= size; m++ )
  {

    m1 = m + 1;
    sxn = -y[ m1 - 1 ];
    sd = -r[ size - 1 ];
    for ( j = 1; j <= m; j++ )
    {

      sxn += r[ size - 1 + m1 - j ] * x[ j - 1 ];
      sd += r[ size - 1 + m1 - j ] * g[ m - j ];

    }

    assert( sd != 0.0 );
    x[ m1 - 1 ] = sxn / sd;
    for ( j = 1; j <= m; j++ )
      x[ j - 1 ] -= x[ m1 - 1 ] * g[ m - j ];
    if ( m1 == size )
    {

      delete [] h;
      delete [] g;
      return;

    }
    sgn = - r[ size - 1 - m1 ];
    shn = - r[ size - 1 + m1 ];
    sgd = - r[ size - 1 ];
    for ( j = 1; j <= m; j++ )
    {

      sgn += r[ size - 1 + j - m1 ] * g[ j - 1 ];
      shn += r[ size - 1 + m1 - j ] * h[ j - 1 ];
      sgd += r[ size - 1 + j - m1 ] * h[ m - j ];

    }

    assert( sd != 0.0 && sgd != 0.0 );
    g[ m1 - 1 ] = sgn / sgd;
    h[ m1 - 1 ] = shn / sd;
    k = m;
    m2 = ( m + 1 ) >> 1;
    pp = g[ m1 - 1 ];
    qq = h[ m1 - 1 ];
    for ( j = 1; j <= m2; j++ )
    {

      pt1 = g[ j - 1 ];
      pt2 = g[ k - 1 ];
      qt1 = h[ j - 1 ];
      qt2 = h[ k - 1 ];
      g[ j - 1 ] = pt1 - pp * qt2;
      g[ k - 1 ] = pt2 - pp * qt1;
      h[ j - 1 ] = qt1 - qq * pt2;
      h[ k - 1 ] = qt2 - qq * pt1;
      k--;

    }

  }

  // should not pass here
  delete [] h;
  delete [] g;
  assert( 0 );

}


void splineInterpolation( const short* orig,
                          short* dest,
                          int size,
                          float res,
                          const AimsVector< float, 3 >& param )
{

  int i, j, k;

  float center = size * res / 2.0;
  float value=0.0f, translation=0.0f, y=0.0f;

  float scale = param[ 0 ];
  float T0 = param[ 1 ];
  float T1 = param[ 2 ];

  float* column = new float[ size ];
  float* filtered = new float[ size ];
  float* r = new float[ 2 * size - 1 ];

  for ( k = 0; k < 2 * size - 1; k++ )
    r[ k ] = 0.0f;
  r[ size - 1 ] = 4.0;
  r[ size - 2 ] = r[ size ] = 1.0;

  for ( i = 0; i < size; i++ )
  {

    for ( j = 0; j < size; j++ )
      column[ j ] = 6.0 * orig[ j * size + i ];

    toeplitz( r, column, filtered, size );

    for ( j = 0; j < size; j++ )
    {

      translation = T0 + T1 * ( center - ( i + 0.5 ) * res );
      y = center - ( center - ( ( j + 0.5 ) * res ) ) * scale + translation;

      for ( value = 0.0f, k = 0; k < size; k++ )
        value += filtered[ k ] * betaSpline3( y / res - 0.5 - k );

      dest[ j * size + i ] = ( short )( value < 0 ? 0 : value );

    }

  }

  delete [] r;
  delete [] filtered;
  delete [] column;

}
