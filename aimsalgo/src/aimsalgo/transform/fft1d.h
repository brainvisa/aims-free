
#ifndef AIMS_TRANSFORM_FFT1D_H
#define AIMS_TRANSFORM_FFT1D_H


#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <typename T> class Volume;
  template <typename T> class VolumeRef;
}


/**@name Fourier transform.
*/
//@{
/**@name 1D Fast Fourier Transform.
         This Fourier transform is a Fast Fourier transform taken
         from the Numerical Recipee in C.
         @param vec 1D-input data of complex float items
         @param isign +1 for direct transform and -1 for inverse transform
*/
//@{
///
carto::VolumeRef< float > AimsXCor1d(
  const carto::rc_ptr<carto::Volume<float> > & data1,
  const carto::rc_ptr<carto::Volume<float> > & data2 );
//@}
/**@name Cross-correlation.
*/
//@{
/**@name 1D Cross-correlation.
         This cross-correlation is based upon Fast Fourier transform taken
         from the Numerical Recipee in C.
         @param vec 1D-input data of complex float items
         @param vec 1D-input data of complex float items
*/
//@{
///
carto::VolumeRef< cfloat >
AimsFFT1D(const carto::rc_ptr<carto::Volume< cfloat > > & vec,int isign=1 );
//@}

/// Linear to decibel scale transformation
carto::VolumeRef<float>
AimsLinearToDecibel( const carto::rc_ptr<carto::Volume<float> > & sqv );

//@}


#endif
