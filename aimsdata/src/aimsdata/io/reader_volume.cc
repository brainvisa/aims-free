// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/reader_d.h>
#include <aims/data/data.h>
#include <aims/rgb/rgb.h>
#include <aims/hsv/hsv.h>

using namespace aims;
using namespace carto;
using namespace std;

#define COMMA ,

AIMS_INSTANTIATE_READER( AimsData<int8_t> );
AIMS_INSTANTIATE_READER( AimsData<uint8_t> );
AIMS_INSTANTIATE_READER( AimsData<int16_t> );
AIMS_INSTANTIATE_READER( AimsData<uint16_t> );
AIMS_INSTANTIATE_READER( AimsData<int32_t> );
AIMS_INSTANTIATE_READER( AimsData<uint32_t> );
AIMS_INSTANTIATE_READER( AimsData<float> );
AIMS_INSTANTIATE_READER( AimsData<double> );
AIMS_INSTANTIATE_READER( AimsData<cfloat> );
AIMS_INSTANTIATE_READER( AimsData<cdouble> );
AIMS_INSTANTIATE_READER( AimsData<AimsRGB> );
AIMS_INSTANTIATE_READER( AimsData<AimsRGBA> );
AIMS_INSTANTIATE_READER( AimsData<AimsHSV> );
AIMS_INSTANTIATE_READER( AimsData<Point3df> );
AIMS_INSTANTIATE_READER( AimsData<Point3d> );
AIMS_INSTANTIATE_READER( AimsData<AimsVector<float COMMA 6> > );

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
AIMS_INSTANTIATE_READER( AimsData<char> );
#endif
AIMS_INSTANTIATE_READER( AimsData<int64_t> );
AIMS_INSTANTIATE_READER( AimsData<uint64_t> );


