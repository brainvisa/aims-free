// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Mesh projection class
 */

#include <aims/meshprojection/meshprojection_d.h>

namespace aims
{

template class MeshProjection< float >;
template class MeshProjection< int16_t >;

template class MeshProjectionFactory< float >;
template class MeshProjectionFactory< int16_t >;

} // namespace aims
