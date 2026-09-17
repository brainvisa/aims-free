

#include "g2dSmoother.h"


Gaussian2dSmoother::Gaussian2dSmoother( float sigma, int levels )
                   : Smoother( levels ),
                     _gCoef( sigma ),
                     _gLin( _gCoef, levels ),
                     _gCol( _gCoef, levels )
{
}


Gaussian2dSmoother::~Gaussian2dSmoother()
{
}
