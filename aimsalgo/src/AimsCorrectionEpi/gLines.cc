

#include "gLines.h"

GaussianLines::GaussianLines( const GCoefficients& coef, int levels )
              : RecursiveGaussian( coef, levels )
{

  _output = new float[ _levels ];
  _work = new float[ _levels ];

}


GaussianLines::~GaussianLines()
{

  delete [] _work;
  delete [] _output;

}


void GaussianLines::doIt( float* p12linear )
{

  float* ptr = p12linear;

  for ( int j = _levels; j--; )
  {

    recurse( ptr, _output, _work );
    memcpy( ptr, _output, _levels * sizeof( float ) );
    ptr += _levels;

  }

}
