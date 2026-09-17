// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/writer_d.h>
#include <aims/mesh/surface.h>

using namespace std;
using namespace carto;
using namespace aims;

#define COMMA ,

AIMS_INSTANTIATE_WRITER( AimsTimeSurface<2 COMMA Void> );
AIMS_INSTANTIATE_WRITER( AimsSurfaceTriangle);
AIMS_INSTANTIATE_WRITER( AimsSurfaceFacet);
AIMS_INSTANTIATE_WRITER( AimsTimeSurface<2 COMMA float> );
AIMS_INSTANTIATE_WRITER( AimsTimeSurface<3 COMMA float> );
AIMS_INSTANTIATE_WRITER( AimsTimeSurface<4 COMMA float> );
AIMS_INSTANTIATE_WRITER( AimsTimeSurface<2 COMMA Point2df> );
AIMS_INSTANTIATE_WRITER( AimsTimeSurface<3 COMMA Point2df> );
AIMS_INSTANTIATE_WRITER( AimsTimeSurface<4 COMMA Point2df> );
