
#ifndef AIMS_SIGNALFILTER_PARITY_H
#define AIMS_SIGNALFILTER_PARITY_H

#include <aims/config/aimsalgo_config.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <class T> class Volume;
  template <class T> class VolumeRef;
}


/**@name Even part of a float vector
         @param xmin inferior bound
         @param xmax superior bound
         @param center center of symmetry
*/
carto::VolumeRef<float>
AimsEvenPart(const carto::rc_ptr<carto::Volume<float> > &vec,
             int xmin,int xmax,int center);

/**@name Odd part of a float vector
         @param xmin inferior bound
         @param xmax superior bound
         @param center center of symmetry
*/
//@{
///
carto::VolumeRef<float>
AimsOddPart(const carto::rc_ptr<carto::Volume<float> > &vec,
            int xmin,int xmax,int center);
//@}

//@}

#endif
