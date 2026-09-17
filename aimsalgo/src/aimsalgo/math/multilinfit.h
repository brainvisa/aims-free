
#ifndef AIMS_MATH_MULTILINFIT_H
#define AIMS_MATH_MULTILINFIT_H

#include <aims/config/aimsalgo_config.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}

/**@name Multilinear least square data fit
    The model of the fit is : \\
    \\yi = a0 + [b0 b1 ...... bM-1].[xi(0) xi(1) ...... xi(M-1)]\\
    \\Given an X matrix containing x vector samples on lines, and given
    the Y vector containing the result of the model for each sample, the
    function returns the vector of coefficients [b0 b1 ....... bM-1].
*/
carto::VolumeRef<float> AimsMultilinearLeastSquareFit(
  const carto::rc_ptr<carto::Volume<float> > &X,
  const carto::rc_ptr<carto::Volume<float> > &Y );


#endif






