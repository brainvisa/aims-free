

#include "gRecursive.h"


RecursiveGaussian::RecursiveGaussian( const GCoefficients& coef, int levels )
                  : _c( coef ),
                    _levels( levels )
{
}


RecursiveGaussian::~RecursiveGaussian()
{
}


void RecursiveGaussian::recurse( float* in, float* out, float* w )
{

  int n = _levels - 4;

  // Calcul des yk+
  float *optr0 = out;
  float *optr1 = out + 1;
  float *optr2 = out + 2;
  float *optr3 = out + 3;
  float *optr4 = out + 4;

  float *iptr0 = in;
  float *iptr1 = in + 1;
  float *iptr2 = in + 2;
  float *iptr3 = in + 3;

  *optr0 = _c.n00() * *iptr0;
  *optr1 = _c.n00() * *iptr1 + _c.n11() * *iptr0 - _c.d11() * *optr0;
  *optr2 = _c.n00() * *iptr2 + _c.n11() * *iptr1 + _c.n22() * *iptr0 - 
           _c.d11() * *optr1 - _c.d22() * *optr0;
  *optr3 = _c.n00() * *iptr3++ + _c.n11() * *iptr2++ + _c.n22() * *iptr1++ +
           _c.n33() * *iptr0++ -
           _c.d11() * *optr2 - _c.d22() * *optr1 - _c.d33() * *optr0;

  for ( ; n--; )
    *optr4++ = _c.n00() * *iptr3++ + _c.n11() * *iptr2++ + _c.n22() * *iptr1++ +
               _c.n33() * *iptr0++ - _c.d11() * *optr3++ - _c.d22() * *optr2++ - 
               _c.d33() * *optr1++ - _c.d44() * *optr0++;

  // Calcul des yk-
  optr0 = w + _levels - 1;
  optr1 = optr0 - 1;
  optr2 = optr0 - 2;
  optr3 = optr0 - 3;
  optr4 = optr0 - 4;

  iptr0 = in + _levels - 1;
  iptr1 = iptr0 - 1;
  iptr2 = iptr0 - 2;
  iptr3 = iptr0 - 3;

  *optr0 = 0.0f;
  *optr1 = _c.n11b() * *iptr0;
  *optr2 = _c.n11b() * *iptr1 + _c.n22b() * *iptr0 - 
           _c.d11() * *optr1 - _c.d22() * *optr0;
  *optr3 = _c.n11b() * *iptr2 + _c.n22b() * *iptr1 + _c.n33b() * *iptr0 - 
           _c.d11() * *optr2 - _c.d22() * *optr1 - _c.d33() * *optr0;

  for ( n = _levels - 4; n--; )
    *optr4-- = _c.n11b() * *iptr3-- + _c.n22b() * *iptr2-- +
               _c.n33b() * *iptr1-- +
               _c.n44b() * *iptr0-- - _c.d11() * *optr3-- - 
               _c.d22() * *optr2-- - 
               _c.d33() * *optr1-- - _c.d44() * *optr0--;

  // Calcul des yk = yk+ + yk-
  optr0 = out;
  iptr0 = w;

  for ( n = _levels; n--; optr0++ )
    *optr0 += *iptr0++;
/*
  for ( int i = 0; i < _levels; i++ )
    out[ i ] = 2 * in[ i ];
*/
}
