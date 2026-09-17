// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/writer_d.h>
#include <aims/io/writer_d.h>
#include <aims/data/data.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>

using namespace std;
using namespace carto;
using namespace aims;

#define COMMA ,

AIMS_INSTANTIATE_WRITER( AimsData<int8_t> );
AIMS_INSTANTIATE_WRITER( AimsData<uint8_t> );
AIMS_INSTANTIATE_WRITER( AimsData<int16_t> );
AIMS_INSTANTIATE_WRITER( AimsData<uint16_t> );
AIMS_INSTANTIATE_WRITER( AimsData<int32_t> );
AIMS_INSTANTIATE_WRITER( AimsData<uint32_t> );
AIMS_INSTANTIATE_WRITER( AimsData<float> );
AIMS_INSTANTIATE_WRITER( AimsData<double> );
AIMS_INSTANTIATE_WRITER( AimsData<cfloat> );
AIMS_INSTANTIATE_WRITER( AimsData<cdouble> );
AIMS_INSTANTIATE_WRITER( AimsData<AimsRGB> );
AIMS_INSTANTIATE_WRITER( AimsData<AimsRGBA> );
AIMS_INSTANTIATE_WRITER( AimsData<AimsHSV> );
AIMS_INSTANTIATE_WRITER( AimsData<Point3df> );
AIMS_INSTANTIATE_WRITER( AimsData<Point3d> );
AIMS_INSTANTIATE_WRITER( AimsData<AimsVector<float COMMA 6> > );

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_WRITER( AimsData<char> );
#endif
AIMS_INSTANTIATE_WRITER( AimsData<int64_t> );
AIMS_INSTANTIATE_WRITER( AimsData<uint64_t> );

