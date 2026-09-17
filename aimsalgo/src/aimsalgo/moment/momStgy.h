
#ifndef AIMS_MOMENT_MOMSTGY_H
#define AIMS_MOMENT_MOMSTGY_H

#include <cartodata/volume/volume.h>
#include <aims/moment/moment.h>


template< class T >
class MomentStrategy
{
  public:
  
    MomentStrategy() { }
    virtual ~MomentStrategy() { }
    
    virtual void update( Moment< T > *, double, double, double, int ) { }
    virtual void doit( Moment< T > *, carto::rc_ptr<carto::Volume< T > > &, T,
                       int ) { }
    virtual void doit( Moment< T > *, const aims::BucketMap<Void> &, int ) { }
};

#endif
