// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/writer_d.h>
#include <aims/bucket/bucket.h>

using namespace std;
using namespace carto;
using namespace aims;

AIMS_INSTANTIATE_WRITER( AimsBucket<Void> );
AIMS_INSTANTIATE_WRITER( BucketMap<Void> );
AIMS_INSTANTIATE_WRITER( AimsBucket<short> );
AIMS_INSTANTIATE_WRITER( BucketMap<short> );
AIMS_INSTANTIATE_WRITER( AimsBucket<uint16_t> );
AIMS_INSTANTIATE_WRITER( BucketMap<uint16_t> );
AIMS_INSTANTIATE_WRITER( AimsBucket<int32_t> );
AIMS_INSTANTIATE_WRITER( BucketMap<int32_t> );
AIMS_INSTANTIATE_WRITER( AimsBucket<uint32_t> );
AIMS_INSTANTIATE_WRITER( BucketMap<uint32_t> );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<int8_t> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<uint8_t> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<int16_t> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<uint16_t> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<int32_t> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<uint32_t> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<float> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<vector<double> > );
AIMS_INSTANTIATE_WRITER( AimsBucket<float> );
AIMS_INSTANTIATE_WRITER( BucketMap<float> );
AIMS_INSTANTIATE_WRITER( AimsBucket<double> );
AIMS_INSTANTIATE_WRITER( BucketMap<double> );
AIMS_INSTANTIATE_WRITER( AimsBucket<DtiTensor> );
AIMS_INSTANTIATE_WRITER( BucketMap<DtiTensor> );
