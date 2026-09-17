
#ifndef AIMS_OPTIMIZATION_COVSRT_H
#define AIMS_OPTIMIZATION_COVSRT_H

#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <class T> class Volume;
}


template < class T >
class CovarianceStorage
{
public:

  CovarianceStorage()  { }
  virtual ~CovarianceStorage() { }

  carto::VolumeRef< T > doit( carto::rc_ptr<carto::Volume< T > >&,
                              carto::rc_ptr<carto::Volume< int > > *ia=NULL,
                              int mfit=0 );
};

#endif
