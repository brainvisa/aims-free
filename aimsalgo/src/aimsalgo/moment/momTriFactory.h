
#ifndef AIMS_MOMENT_MOMTRIFACTORY_H
#define AIMS_MOMENT_MOMTRIFACTORY_H

#include <aims/config/aimsalgo_config.h>
#include <aims/moment/momBase.h>
#include <aims/moment/momTriStgy.h>


class AIMSALGO_API MomentTriangleFactory
{
  public:
  
    MomentTriangleFactory() { }
    virtual ~MomentTriangleFactory() { }
    
    MomentTriangleStrategy *create( MomentBase::MomentType );
};

#endif
