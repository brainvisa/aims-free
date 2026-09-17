#ifndef AIMS_DATA_VOLUMEOPERATORS_AIMS_H
#define AIMS_DATA_VOLUMEOPERATORS_AIMS_H

#include <cartodata/volume/volumeoperators.h>
#include <aims/vector/vector.h>
#include <aims/math/dtitensor.h>

namespace carto
{


  template <typename T, int D, typename U>
  class Divider<AimsVector<T, D>, U>
  {
  public:
    inline Divider( U x ) : divisor( x ) {}
    inline AimsVector<T, D> operator () ( const AimsVector<T, D> & x ) const
    { return x * ( 1. / divisor ); }
    U      divisor;
  };

  DECLARE_GENERIC_OBJECT_TYPE( VolumeRef< DtiTensor* > )
  DECLARE_GENERIC_OBJECT_TYPE( rc_ptr<Volume< DtiTensor* > > )

}

#endif

