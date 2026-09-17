
#ifndef AIMS_SIGNALFILTER_GFRAMES_H
#define AIMS_SIGNALFILTER_GFRAMES_H

#include <aims/config/aimsalgo_config.h>
#include <aims/signalfilter/grecurse.h>
#include <cartodata/volume/volume.h>


class GaussianFrames : public RecursiveGaussian
{
public:

  GaussianFrames() : RecursiveGaussian() { }

  void doit( carto::rc_ptr<carto::Volume<float> > & );
  void doit( carto::rc_ptr<carto::Volume<float> > &, const GCoef& );
};


#endif
