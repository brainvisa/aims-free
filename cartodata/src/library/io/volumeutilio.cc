//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeutilio_d.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/voxelrgb.h>
#include <cartobase/type/voxelrgba.h>
#include <cartobase/type/voxelhsv.h>
//----------------------------------------------------------------------------

using namespace soma;
using namespace carto;

namespace soma {

template class VolumeUtilIO<int8_t>;
template class VolumeUtilIO<int16_t>;
template class VolumeUtilIO<int32_t>;
template class VolumeUtilIO<int64_t>;
template class VolumeUtilIO<uint8_t>;
template class VolumeUtilIO<uint16_t>;
template class VolumeUtilIO<uint32_t>;
template class VolumeUtilIO<uint64_t>;
template class VolumeUtilIO<float>;
template class VolumeUtilIO<double>;
template class VolumeUtilIO<VoxelRGB>;
template class VolumeUtilIO<VoxelRGBA>;
template class VolumeUtilIO<VoxelHSV>;
template class VolumeUtilIO<std::complex<float> >;
template class VolumeUtilIO<std::complex<double> >;
template class VolumeUtilIO<Point3df>;
template class VolumeUtilIO<Point3d>;
template class VolumeUtilIO<Point2d>;
template class VolumeUtilIO<AimsVector<float, 6> >;
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
// char may be a different type from int8_t and uint8_t
template class VolumeUtilIO<char>;
#endif

}
