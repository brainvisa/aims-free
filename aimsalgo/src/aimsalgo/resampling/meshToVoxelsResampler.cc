// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/resampling/meshToVoxelsResampler_d.h>

namespace aims {

template class MeshToVoxelsResampler<carto::rc_ptr<carto::Volume<unsigned int> > >;
template class MeshToVoxelsResampler<BucketMap<Void> >;

} // namespace aims
