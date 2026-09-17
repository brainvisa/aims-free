
#ifndef AIMS_SIGNALFILTER_GLINES_H
#define AIMS_SIGNALFILTER_GLINES_H

#include <aims/config/aimsalgo_config.h>
#include <aims/signalfilter/grecurse.h>
#include <cartodata/volume/volume.h>


class GaussianLines : public RecursiveGaussian
{
public:

  GaussianLines() : RecursiveGaussian() { }

  void doit( carto::rc_ptr<carto::Volume<float> >& );
  void doit( carto::rc_ptr<carto::Volume<float> >&, const GCoef& );
};


#endif
