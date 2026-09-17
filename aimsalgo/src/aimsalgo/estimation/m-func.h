
#ifndef AIMS_ESTIMATION_M_FUNC_H
#define AIMS_ESTIMATION_M_FUNC_H

#include <aims/config/aimsalgo_config.h>


//
// M-estimator function base class
//
class MEstimatorFunc
{
  public:
    MEstimatorFunc() : _sigma( 1.0 ), _sigma2( 1.0 ) { }
    virtual ~MEstimatorFunc() { }

    virtual float norm( float ) const { return 0.0; }
    virtual float influence( float ) const { return 0.0; }
    virtual float weight( float ) const { return 0.0; }

    virtual void setSigma( float sigma )
    {
      _sigma = sigma;
      _sigma2 = sigma * sigma;
    }
    float sigma() const { return _sigma; }

  protected:
    float _sigma;
    float _sigma2;
};


#endif

