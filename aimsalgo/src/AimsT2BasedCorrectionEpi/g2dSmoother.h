

#ifndef _AIMSCORRECTIONEPI_G2DSMOOTHER_H_
#define _AIMSCORRECTIONEPI_G2DSMOOTHER_H_

#include "smoother.h"
#include "gLines.h"
#include "gColumns.h"


class Gaussian2dSmoother : public Smoother
{

  public:

    Gaussian2dSmoother( float sigma, int levels );
    virtual ~Gaussian2dSmoother();

    void doIt( float* p12linear );

  protected:

    GCoefficients _gCoef;
    GaussianLines _gLin;
    GaussianColumns _gCol;

};


inline
void Gaussian2dSmoother::doIt( float* p12linear )
{

  _gLin.doIt( p12linear );
  _gCol.doIt( p12linear );

}


#endif
