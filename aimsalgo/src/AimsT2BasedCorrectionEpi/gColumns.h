

#ifndef _AIMSCORRECTIONEPI_GCOLUMNS_H_
#define _AIMSCORRECTIONEPI_GCOLUMNS_H_


#include "gCoefficients.h"
#include "gRecursive.h"


class GaussianColumns : public RecursiveGaussian
{

  public:

    GaussianColumns( const GCoefficients& coef, int levels );
    virtual ~GaussianColumns();

    void doIt( float* p12linear );

  private:

    float* _input;
    float* _output;
    float* _work;

};



#endif
