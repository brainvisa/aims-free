
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/transform/hilbert1d.h>
#include <cartodata/volume/volume.h>
#include <aims/math/mathelem.h>
#include <aims/transform/fft1d.h>
#include <math.h>
#include <complex>

using namespace carto;
using namespace std;

VolumeRef<float>
AimsHilbert1D(const rc_ptr<Volume<float> > & vec)
{
  int                x;
  int                newdim;

  newdim = (vec->getSizeX()==AimsNextPowerOfTwo(vec->getSizeX())/2) ?
            vec->getSizeX() : AimsNextPowerOfTwo(vec->getSizeX());

  VolumeRef<float> vec1(newdim);
  VolumeRef< cfloat > cvec(newdim);
  for (x=0;x<cvec->getSizeX();x++)
     cvec(x) = (*vec1)(x);

  cvec = AimsFFT1D(cvec,1);

  for (x=0;x<=cvec->getSizeX()/2;x++)
    cvec(x) *= (float)2.0;

  for (x=cvec->getSizeX()/2+1;x<cvec->getSizeX();x++)
    cvec(x) = 0.0;

  cvec = AimsFFT1D(cvec,-1);

  VolumeRef<float> vec2(vec->getSizeX());
  for (x=0;x<cvec->getSizeX();x++)
     vec2(x) = real(cvec(x));
  
  return(vec2);
}
