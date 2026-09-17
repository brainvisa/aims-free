
#ifndef AIMS_MOMENT_MOMSURFSTGY_H
#define AIMS_MOMENT_MOMSURFSTGY_H

#include <aims/config/aimsalgo_config.h>
#include <aims/moment/momTriStgy.h>


class MomentSurfacicStrategy : public MomentTriangleStrategy
{
  public:
  
    MomentSurfacicStrategy() : MomentTriangleStrategy() { }
    
    void update( Moment< Void > *, Point3df&, Point3df&, Point3df& );
    void doit( Moment< Void > *, AimsSurfaceTriangle& );
};

#endif
