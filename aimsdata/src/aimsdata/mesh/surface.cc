// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cartobase/object/object_d.h>
#include <aims/mesh/surface.h>

namespace carto
{

INSTANTIATE_GENERIC_OBJECT_TYPE( AimsSegments )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsSurfaceTriangle )
INSTANTIATE_GENERIC_OBJECT_TYPE( AimsSurfaceFacet )

INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsSegments > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsSurfaceTriangle > )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< AimsSurfaceFacet > )

#define _mesh_type AimsTimeSurface<2, float>
INSTANTIATE_GENERIC_OBJECT_TYPE( _mesh_type )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<3, float>
INSTANTIATE_GENERIC_OBJECT_TYPE( _mesh_type )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<4, float>
INSTANTIATE_GENERIC_OBJECT_TYPE( _mesh_type )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<2, Point2df>
INSTANTIATE_GENERIC_OBJECT_TYPE( _mesh_type )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<3, Point2df>
INSTANTIATE_GENERIC_OBJECT_TYPE( _mesh_type )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type
#define _mesh_type AimsTimeSurface<4, Point2df>
INSTANTIATE_GENERIC_OBJECT_TYPE( _mesh_type )
INSTANTIATE_GENERIC_OBJECT_TYPE( rc_ptr< _mesh_type > )
#undef _mesh_type

} // namespace carto
