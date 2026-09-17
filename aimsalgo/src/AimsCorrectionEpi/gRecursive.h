

#ifndef _AIMSCORRECTIONEPI_GRECURSIVE_H_
#define _AIMSCORRECTIONEPI_GRECURSIVE_H_


#include "gCoefficients.h"


class RecursiveGaussian
{

  public:

    RecursiveGaussian( const GCoefficients& coef, int levels );
    virtual ~RecursiveGaussian();

    void recurse( float* in, float* out, float* w );

  protected:

    const GCoefficients& _c;
    int _levels;

};


#endif
