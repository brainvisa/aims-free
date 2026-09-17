// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/reader_d.h>
#include <aims/bucket/bucket.h>
#include <aims/math/dtitensor.h>

using namespace aims;
using namespace carto;
using namespace std;

AIMS_INSTANTIATE_READER( AimsBucket<Void> );
AIMS_INSTANTIATE_READER( BucketMap<Void> );
AIMS_INSTANTIATE_READER( AimsBucket<short> );
AIMS_INSTANTIATE_READER( BucketMap<short> );
AIMS_INSTANTIATE_READER( AimsBucket<uint16_t> );
AIMS_INSTANTIATE_READER( BucketMap<uint16_t> );
AIMS_INSTANTIATE_READER( AimsBucket<int32_t> );
AIMS_INSTANTIATE_READER( BucketMap<int32_t> );
AIMS_INSTANTIATE_READER( AimsBucket<uint32_t> );
AIMS_INSTANTIATE_READER( BucketMap<uint32_t> );
AIMS_INSTANTIATE_READER( AimsBucket<vector<int8_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<uint8_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<int16_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<uint16_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<int32_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<uint32_t> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<float> > );
AIMS_INSTANTIATE_READER( AimsBucket<vector<double> > );
AIMS_INSTANTIATE_READER( AimsBucket<float> );
AIMS_INSTANTIATE_READER( BucketMap<float> );
AIMS_INSTANTIATE_READER( AimsBucket<double> );
AIMS_INSTANTIATE_READER( BucketMap<double> );
AIMS_INSTANTIATE_READER( AimsBucket<DtiTensor> );
AIMS_INSTANTIATE_READER( BucketMap<DtiTensor> );
