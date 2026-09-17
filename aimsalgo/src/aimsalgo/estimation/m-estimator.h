
#ifndef AIMS_ESTIMATION_M_ESTIMATOR_H
#define AIMS_ESTIMATION_M_ESTIMATOR_H

#include <aims/config/aimsalgo_config.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <class T> class Volume;
  template <class T> class VolumeRef;
}

template <class T, int D> class AimsVector;

//
// M-estimator base class
//
template < int D >
class MEstimator
{
  public:
    MEstimator() { }
    virtual ~MEstimator() { }

    virtual 
    void doit( const carto::rc_ptr<carto::Volume< AimsVector< float, D > > >&,
               const carto::rc_ptr<carto::Volume< float > >&,
               float&, AimsVector< float, D >& ) { }

};


#endif
