
#ifndef AIMS_OPTIMIZATION_LEVMRQ_H
#define AIMS_OPTIMIZATION_LEVMRQ_H

#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>
#include <aims/math/gaussj.h>
#include <aims/optimization/lmfunc.h>
#include <aims/optimization/covsrt.h>
#include <cartobase/smart/rcptr.h>

namespace carto
{
  template <class T> class Volume;
  template <class T> class VolumeRef;
}


template < class T >
class LevenbergMarquardt
{
public:

  LevenbergMarquardt( LMFunction< T > *lmf )  { lmFonc = lmf; }
  virtual ~LevenbergMarquardt() { }

  LMFunction< T > *doit( carto::rc_ptr<carto::Volume< T > >&,
                         carto::rc_ptr<carto::Volume< T > >&,
                         carto::rc_ptr<carto::Volume< T > > *sig=NULL,
                         carto::rc_ptr<carto::Volume< int > > *ia=NULL,
                         carto::rc_ptr<carto::Volume< T > > *covar=NULL );

private:

  GaussJordan< T > gaussj;
  CovarianceStorage< T > covsrt;

  LMFunction< T > *lmFonc;

  bool mrqmin( carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< int > >&, T *, T *,
               carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< T > >& );

  void mrqcof( carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< T > >&,
	       carto::rc_ptr<carto::Volume< int > >&, T *,
               carto::rc_ptr<carto::Volume< T > >&,
               carto::rc_ptr<carto::Volume< T > >& );
};

#endif
