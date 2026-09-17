
#ifndef AIMS_MOMENT_TRIMOMENT_H
#define AIMS_MOMENT_TRIMOMENT_H

#include <aims/config/aimsalgo_config.h>
#include <aims/vector/vector.h>

#include <aims/moment/moment.h>
#include <aims/moment/momBase.h>
#include <aims/moment/momTriStgy.h>


class TriangulationMoment
  : public MomentBase, public Moment< int16_t >
{
  public:
  
    TriangulationMoment( MomentType mType=Volumic );

    void setMomentType( MomentType );
  
    // ignore this warning (the API is broken, see comment in Moment<T>)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Woverloaded-virtual"
    virtual void doit( AimsSurfaceTriangle& );
    #pragma GCC diagnostic pop
    
  private:
  
    MomentTriangleStrategy *stgy;
};

#endif
