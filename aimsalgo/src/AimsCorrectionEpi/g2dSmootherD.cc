
#include "g2dSmootherD.h"
#include <iostream>
#include <math.h>
#include <string.h>

using namespace std;

DiscreteGaussian2dSmoother::DiscreteGaussian2dSmoother( int sizeMask,
                                                        int levels )
                           : Smoother( levels ),
                             _coefCount( ( sizeMask + 1 ) / 2 )
{

  _levels2 = _levels * _levels;
  _coef = new float[ _coefCount ];
  _tmp = new float[ _levels2 ];

  // never use _coef[ 0 ] = 1.0 !
  for ( int m = 1; m < _coefCount; m++ )
    _coef[ m ] = exp( - 0.5 * m * m );

}


DiscreteGaussian2dSmoother::~DiscreteGaussian2dSmoother()
{

  delete [] _tmp;
  delete [] _coef;

}


void DiscreteGaussian2dSmoother::doIt( float* p12linear )
{

  float* in = p12linear;
  float* out = _tmp;

  float* pIn = 0;
  float* pOut = 0;

  int finalIndex = 0;
  int l = 0;
  int m = 0;
  float coef;

  // smoothing of lines
  for ( int line = 0; line < _levels; line++ )
  {

    pIn = in;
    pOut = out;
    for ( l = 0; l < _levels; l++ )
      *pOut++ = *pIn++;


    for ( m = 1; m < _coefCount; m++ )
    {

      coef = _coef[ m ];
      pIn = in + m;
      pOut = out;
      finalIndex = _levels - m;
      for ( l = 0; l < finalIndex; l++ )
        *pOut++ += coef * *pIn++;

      pIn = in;
      pOut = out + m;
      for ( l = m; l < _levels; l++ )
        *pOut++ += coef * *pIn++;

    }

    in += _levels;
    out += _levels;


  }

  in = _tmp;
  out = p12linear;

  // smoothing of columns
  for ( int column = 0; column < _levels; column++ )
  {

    pIn = in;
    pOut = out;
    for ( l = 0; l < _levels; l++, pOut += _levels, pIn += _levels )
      *pOut = *pIn;

    int offset = _levels;
    for ( m = 1; m < _coefCount; m++ )
    {

      coef = _coef[ m ];
      pIn = in + offset;
      pOut = out;
      finalIndex = _levels - m;
      for ( l = 0; l < finalIndex; l++, pOut += _levels, pIn += _levels )
        *pOut += coef * *pIn;

      pIn = in;
      pOut = out + offset;
      for ( l = m; l < _levels; l++, pOut += _levels, pIn += _levels )
        *pOut += coef * *pIn;

      offset += _levels;

    }

    in ++;
    out ++;

  }

}
