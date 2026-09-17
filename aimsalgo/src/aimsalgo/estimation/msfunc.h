
#ifndef AIMS_ESTIMATION_MSFUNC_H
#define AIMS_ESTIMATION_MSFUNC_H


#include <aims/config/aimsalgo_config.h>
#include <aims/estimation/m-func.h>
#include <aims/math/mathelem.h>


//
// Mean Squared M-estimator function
//
class MSFunc : public MEstimatorFunc
{
  public:
    MSFunc() : MEstimatorFunc() { }
    ~MSFunc() { }

    float norm( float error ) const;
    float influence( float error ) const;
    float weight( float error ) const;
};


inline
float MSFunc::norm( float error ) const
{
  return aims::sqr( error ) / ( 2.0 * _sigma2 );
}


inline
float MSFunc::influence( float error ) const
{
  return error / _sigma2;
}


inline
float MSFunc::weight( float ) const
{
  return 1 / _sigma2;
}


#endif

