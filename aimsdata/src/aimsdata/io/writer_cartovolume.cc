// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <aims/io/writer_d.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>
#include <cartodata/volume/volume.h>

using namespace std;
using namespace carto;
using namespace aims;

#define COMMA ,

AIMS_INSTANTIATE_WRITER( Volume<int8_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint8_t> );
AIMS_INSTANTIATE_WRITER( Volume<int16_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint16_t> );
AIMS_INSTANTIATE_WRITER( Volume<int32_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint32_t> );
AIMS_INSTANTIATE_WRITER( Volume<float> );
AIMS_INSTANTIATE_WRITER( Volume<double> );
AIMS_INSTANTIATE_WRITER( Volume<cfloat> );
AIMS_INSTANTIATE_WRITER( Volume<cdouble> );
AIMS_INSTANTIATE_WRITER( Volume<AimsRGB> );
AIMS_INSTANTIATE_WRITER( Volume<AimsRGBA> );
AIMS_INSTANTIATE_WRITER( Volume<AimsHSV> );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_WRITER( Volume<char> );
#endif
AIMS_INSTANTIATE_WRITER( Volume<int64_t> );
AIMS_INSTANTIATE_WRITER( Volume<uint64_t> );
AIMS_INSTANTIATE_WRITER( Volume<Point3df> );
AIMS_INSTANTIATE_WRITER( Volume<Point3d> );
AIMS_INSTANTIATE_WRITER( Volume<AimsVector<float COMMA 6> > );

AIMS_INSTANTIATE_WRITER( VolumeRef<int8_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint8_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<int16_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint16_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<int32_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint32_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<float> );
AIMS_INSTANTIATE_WRITER( VolumeRef<double> );
AIMS_INSTANTIATE_WRITER( VolumeRef<cfloat> );
AIMS_INSTANTIATE_WRITER( VolumeRef<cdouble> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsRGB> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsRGBA> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsHSV> );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_WRITER( VolumeRef<char> );
#endif
AIMS_INSTANTIATE_WRITER( VolumeRef<int64_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<uint64_t> );
AIMS_INSTANTIATE_WRITER( VolumeRef<Point3df> );
AIMS_INSTANTIATE_WRITER( VolumeRef<Point3d> );
AIMS_INSTANTIATE_WRITER( VolumeRef<AimsVector<float COMMA 6> > );

