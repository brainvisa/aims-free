
#ifndef AIMS_SIGNALFILTER_GRECURSE_H
#define AIMS_SIGNALFILTER_GRECURSE_H

#include <aims/config/aimsalgo_config.h>
#include <aims/signalfilter/gcoef.h>


class AIMSALGO_API RecursiveGaussian
{
public:

  RecursiveGaussian() { initialize( GCoef() ); }
  virtual ~RecursiveGaussian() { }

  void initialize( const GCoef& );

  void recurse( float *, float *, float *, int );

private:

  float n00;
  float n11;
  float n22;
  float n33;

  float n11b;
  float n22b;
  float n33b;
  float n44b;

  float d11;
  float d22;
  float d33;
  float d44;
};


#endif
