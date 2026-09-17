
#include <aims/moment/momTriFactory.h>
#include <aims/moment/momVolStgy.h>
#include <aims/moment/momSurfStgy.h>


MomentTriangleStrategy *
MomentTriangleFactory::create( MomentBase::MomentType id )
{
  switch( id )
    {
      case MomentBase::Volumic:
        return new MomentVolumicStrategy();
	break;
      case MomentBase::Surfacic:
        return new MomentSurfacicStrategy();
	break;
      default:
        return (MomentTriangleStrategy *)0;
	break;
    }
}
