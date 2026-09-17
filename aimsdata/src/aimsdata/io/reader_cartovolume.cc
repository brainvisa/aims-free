// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <aims/io/reader_d.h>
#include <cartodata/volume/volume.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>

using namespace aims;
using namespace carto;
using namespace std;

#define COMMA ,

AIMS_INSTANTIATE_AIMS_READER( Volume<int8_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<uint8_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<int16_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<uint16_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<int32_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<uint32_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<float> );
AIMS_INSTANTIATE_AIMS_READER( Volume<double> );
AIMS_INSTANTIATE_AIMS_READER( Volume<cfloat> );
AIMS_INSTANTIATE_AIMS_READER( Volume<cdouble> );
AIMS_INSTANTIATE_AIMS_READER( Volume<AimsRGB> );
AIMS_INSTANTIATE_AIMS_READER( Volume<AimsRGBA> );
AIMS_INSTANTIATE_AIMS_READER( Volume<AimsHSV> );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_AIMS_READER( Volume<char> );
#endif
AIMS_INSTANTIATE_AIMS_READER( Volume<int64_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<uint64_t> );
AIMS_INSTANTIATE_AIMS_READER( Volume<Point3df> );
AIMS_INSTANTIATE_AIMS_READER( Volume<Point3d> );
AIMS_INSTANTIATE_AIMS_READER( Volume<AimsVector<float COMMA 6> > );

AIMS_INSTANTIATE_AIMS_READER( VolumeRef<int8_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<uint8_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<int16_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<uint16_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<int32_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<uint32_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<float> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<double> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<cfloat> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<cdouble> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<AimsRGB> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<AimsRGBA> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<AimsHSV> );
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<char> );
#endif
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<int64_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<uint64_t> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<Point3df> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<Point3d> );
AIMS_INSTANTIATE_AIMS_READER( VolumeRef<AimsVector<float COMMA 6> > );

