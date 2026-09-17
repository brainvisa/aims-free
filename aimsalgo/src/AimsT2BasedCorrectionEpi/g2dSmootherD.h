

#ifndef _AIMSCORRECTIONEPI_G2DSMOOTHERD_H_
#define _AIMSCORRECTIONEPI_G2DSMOOTHERD_H_


#include "smoother.h"


class DiscreteGaussian2dSmoother : public Smoother
{

  public:

    DiscreteGaussian2dSmoother( int sizeMask, int levels );
    virtual ~DiscreteGaussian2dSmoother();

    void doIt( float* p12linear );

  protected:

    int _coefCount;
    int _levels2;
    float* _coef;
    float* _tmp;

};


#endif
