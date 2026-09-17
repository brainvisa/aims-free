
#ifndef AIMS_SIGNALFILTER_GSLICES_H
#define AIMS_SIGNALFILTER_GSLICES_H

#include <aims/config/aimsalgo_config.h>
#include <aims/signalfilter/grecurse.h>
#include <cartodata/volume/volume.h>


class AIMSALGO_API GaussianSlices : public RecursiveGaussian
{
public:

  GaussianSlices() : RecursiveGaussian() { }

  void doit( carto::rc_ptr<carto::Volume<float> >& );
  void doit( carto::rc_ptr<carto::Volume<float> >&, const GCoef& );
};


#endif
