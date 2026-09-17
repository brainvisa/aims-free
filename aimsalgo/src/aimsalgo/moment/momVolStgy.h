
#ifndef AIMS_MOMENT_MOMVOLSTGY_H
#define AIMS_MOMENT_MOMVOLSTGY_H

#include <aims/config/aimsalgo_config.h>
#include <aims/moment/momTriStgy.h>


class MomentVolumicStrategy : public MomentTriangleStrategy
{
  public:
  
    MomentVolumicStrategy() : MomentTriangleStrategy() { }
    
    void update( Moment< Void > *, Point3df&, Point3df&, Point3df& );
    void doit( Moment< Void > *, AimsSurfaceTriangle& );
};

#endif
