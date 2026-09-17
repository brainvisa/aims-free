

#ifndef _AIMSCORRECTIONEPI_GLINES_H_
#define _AIMSCORRECTIONEPI_GLINES_H_

#include <string.h>
#include "gCoefficients.h"
#include "gRecursive.h"


class GaussianLines : public RecursiveGaussian
{

  public:

    GaussianLines( const GCoefficients& coef, int levels );
    virtual ~GaussianLines();

    void doIt( float* p12linear );

  private:

    float* _output;
    float* _work;

};


#endif
