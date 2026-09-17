
#ifndef AIMS_TRANSFORM_HILBERT1D_H
#define AIMS_TRANSFORM_HILBERT1D_H

#include <aims/config/aimsalgo_config.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <class T> class Volume;
  template <class T> class VolumeRef;
}
/**@name Hilbert transform.
*/
//@{
/**@name 1D hilbert transform.
         This transform is often used to get the envelop of a signal.
         It is based on the construction of the associated analytic signal.
         @param vec input float 1D-signal
*/
//@{
///
carto::VolumeRef<float>
AimsHilbert1D( const carto::rc_ptr<carto::Volume<float> > &vec );
//@}

//@}

#endif
