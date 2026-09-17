
#ifndef AIMS_MOMENT_MOMTRISTGY_H
#define AIMS_MOMENT_MOMTRISTGY_H

#include <aims/config/aimsalgo_config.h>
#include <aims/moment/moment.h>


class MomentTriangleStrategy
{
  public:
  
    MomentTriangleStrategy() { }
    virtual ~MomentTriangleStrategy() { }
    
    void computeFirst( Point3df&, Point3df&, Point3df& );

    virtual void doit( Moment< Void > *, AimsSurfaceTriangle& ) { }

  protected: 

    double _mt1[ 3 ];
    double _mt2[ 6 ];
    double _mt3[ 10 ];
};

#endif
