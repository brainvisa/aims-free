// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/reader_d.h>
#include <aims/mesh/surface.h>

using namespace aims;
using namespace carto;
using namespace std;

#define COMMA ,

AIMS_INSTANTIATE_READER( AimsTimeSurface<2 COMMA Void> );
AIMS_INSTANTIATE_READER( AimsSurfaceTriangle);
AIMS_INSTANTIATE_READER( AimsSurfaceFacet);
AIMS_INSTANTIATE_READER( AimsTimeSurface<2 COMMA float> );
AIMS_INSTANTIATE_READER( AimsTimeSurface<3 COMMA float> );
AIMS_INSTANTIATE_READER( AimsTimeSurface<4 COMMA float> );
AIMS_INSTANTIATE_READER( AimsTimeSurface<2 COMMA Point2df> );
AIMS_INSTANTIATE_READER( AimsTimeSurface<3 COMMA Point2df> );
AIMS_INSTANTIATE_READER( AimsTimeSurface<4 COMMA Point2df> );

