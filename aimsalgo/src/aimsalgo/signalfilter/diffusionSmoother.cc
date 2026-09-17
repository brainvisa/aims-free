
#include <aims/signalfilter/diffusionSmoother_d.h>

using namespace carto;

namespace aims {

template class DiffusionSmoother<int16_t>;
template class DiffusionSmoother<float>;
template class MaskedDiffusionSmoother<int16_t, std::vector<Point3df> >;
template class MaskedDiffusionSmoother<float, std::vector<Point3df> >;
template class MaskedDiffusionSmoother<int16_t, VolumeRef<short> >;
template class MaskedDiffusionSmoother<float, VolumeRef<short> >;

} // namespace aims
