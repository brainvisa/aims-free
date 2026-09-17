// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/writer_d.h>
#include <aims/mesh/texture.h>

using namespace std;
using namespace carto;
using namespace aims;

AIMS_INSTANTIATE_WRITER( Texture1d);
AIMS_INSTANTIATE_WRITER( Texture2d);
AIMS_INSTANTIATE_WRITER( TimeTexture<double> );
AIMS_INSTANTIATE_WRITER( TimeTexture<int8_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<uint8_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<int16_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<uint16_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<int32_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<uint32_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<int64_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<uint64_t> );
AIMS_INSTANTIATE_WRITER( TimeTexture<Point2d> );
