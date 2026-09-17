
#ifndef AIMS_ESTIMATION_MEDFUNC_H
#define AIMS_ESTIMATION_MEDFUNC_H


#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/estimation/m-func.h>
#include <aims/math/mathelem.h>


//
// Median M-estimator function
//
class MedFunc : public MEstimatorFunc
{
  public:
    MedFunc() : MEstimatorFunc() { }
    ~MedFunc() { }

    float norm( float error ) const;
    float influence( float error ) const;
    float weight( float error ) const;
};


inline
float MedFunc::norm( float error ) const
{
  return fabs( error );
}


inline
float MedFunc::influence( float error ) const
{
  if ( error < 0 )
    return -1.0;
  else if ( error > 0 )
    return +1.0;
  else
    return 0.0;
}


inline
float MedFunc::weight( float error ) const
{
  if ( error )
    return influence( error ) / error;
  else
    return 1.0;
}


#endif
