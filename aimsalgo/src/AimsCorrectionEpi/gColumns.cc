

#include "gColumns.h"


GaussianColumns::GaussianColumns( const GCoefficients& coef, int levels )
              : RecursiveGaussian( coef, levels )
{

  _input = new float[ _levels ];
  _output = new float[ _levels ];
  _work = new float[ _levels ];

}


GaussianColumns::~GaussianColumns()
{

  delete [] _work;
  delete [] _output;
  delete [] _input;

}


void GaussianColumns::doIt( float* p12linear )
{

  float* in = 0;
  float* p1 = p12linear;
  float* ptr = 0;
  float* out = 0;

  int i, j;
  for ( i = _levels; i--; )
  {

    in = _input;
    ptr = p1;
    for ( j = _levels; j--; ptr += _levels )
      *in++ = *ptr;

    recurse( _input, _output, _work );

    out = _output;
    ptr = p1++;
    for ( j = _levels; j--; ptr += _levels )
      *ptr = *out++;

  }

}
