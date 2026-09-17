// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/reader_d.h>
#include <aims/mesh/texture.h>

using namespace aims;
using namespace carto;
using namespace std;

AIMS_INSTANTIATE_READER( Texture1d);
AIMS_INSTANTIATE_READER( Texture2d);
AIMS_INSTANTIATE_READER( TimeTexture<double> );
AIMS_INSTANTIATE_READER( TimeTexture<int8_t> );
AIMS_INSTANTIATE_READER( TimeTexture<uint8_t> );
AIMS_INSTANTIATE_READER( TimeTexture<int16_t> );
AIMS_INSTANTIATE_READER( TimeTexture<uint16_t> );
AIMS_INSTANTIATE_READER( TimeTexture<int32_t> );
AIMS_INSTANTIATE_READER( TimeTexture<uint32_t> );
AIMS_INSTANTIATE_READER( TimeTexture<int64_t> );
AIMS_INSTANTIATE_READER( TimeTexture<uint64_t> );
AIMS_INSTANTIATE_READER( TimeTexture<Point2d> );
