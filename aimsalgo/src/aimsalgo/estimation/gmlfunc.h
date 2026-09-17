
#ifndef AIMS_ESTIMATION_GMLFUNC_H
#define AIMS_ESTIMATION_GMLFUNC_H


#include <aims/config/aimsalgo_config.h>
#include <aims/estimation/m-func.h>
#include <aims/math/mathelem.h>


//
// Geman/McLure M-estimator function
//
class AIMSALGO_API GMLFunc : public MEstimatorFunc
{
  public:
    GMLFunc() : MEstimatorFunc() { }
    ~GMLFunc() { }

    float norm( float error ) const;
    float influence( float error ) const;
    float weight( float error ) const;
};


inline
float GMLFunc::norm( float error ) const
{
  float e2 = aims::sqr( error );
  return e2 / ( e2 + _sigma2 );
}


inline
float GMLFunc::influence( float error ) const
{
  return 2.0 * _sigma2 * error / ( aims::sqr( aims::sqr( error ) + _sigma2 ) ); 
}


inline
float GMLFunc::weight( float error ) const
{
  if ( error )
    return influence( error ) / error;
  else
    return 2.0 / _sigma2;
}


#endif
