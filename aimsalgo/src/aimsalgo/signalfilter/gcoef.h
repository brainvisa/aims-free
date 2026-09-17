
#ifndef AIMS_SIGNALFILTER_GCOEF_H
#define AIMS_SIGNALFILTER_GCOEF_H

#include <math.h>


class GCoef
{

public:

  enum GOrder
  {
    smoothing,
    gradient,
    laplacian
  };

  GCoef( float s = 1.0f, GOrder o = smoothing ) { initialize( s, o ); }
  virtual ~GCoef() { }

  void initialize( float, GOrder );

  float n00() const  { return _n00; }
  float n11() const { return _n11; }
  float n22() const { return _n22; }
  float n33() const { return _n33; }

  float n11b() const { return _n11b; }
  float n22b() const { return _n22b; }
  float n33b() const { return _n33b; }
  float n44b() const { return _n44b; }

  float d11() const { return _d11; }
  float d22() const { return _d22; }
  float d33() const { return _d33; }
  float d44() const { return _d44; }

private:

  float a0;
  float a1;
  float c0;
  float c1;
  float b0;
  float b1;
  float w0;
  float w1;

  float _n00;
  float _n11;
  float _n22;
  float _n33;

  float _n11b;
  float _n22b;
  float _n33b;
  float _n44b;

  float _d11;
  float _d22;
  float _d33;
  float _d44;

  void fillSmoothing( float );
  void fillGradient( float );
  void fillLaplacian( float );

};


#endif
