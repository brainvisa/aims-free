
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/signalfilter/parity.h>
#include <cartodata/volume/volume.h>

using namespace carto;


VolumeRef<float>
AimsEvenPart(const rc_ptr<Volume<float> > &vec,
             int xmin,int xmax,int center)
{
  VolumeRef<float> even( vec->getSize(), vec->getBorders() );
  even = 0.0;

  int dx = vec->getSizeX();
  for (int x=xmin;x<=xmax;x++)
  { if (x<dx)
      even(x) += 0.5 * vec->at(x);
    if ((2*center-x>=0) &&
        (2*center-x<dx))
      even(x) += 0.5 * vec->at(2*center-x);
  }
  return even;
}


VolumeRef<float>
AimsOddPart(const rc_ptr<Volume<float> > &vec,
            int xmin,int xmax,int center)
{
  VolumeRef<float> odd( vec->getSize(), vec->getBorders() );
  odd = 0.0;

  int dx = vec->getSizeX();
  for (int x=xmin;x<=xmax;x++)
  { if (x<dx)
      odd(x) += 0.5 * vec->at(x);
    if ((2*center-x>=0) &&
        (2*center-x<dx))
      odd(x) -= 0.5 * vec->at(2*center-x);
  }
  return odd;
}
