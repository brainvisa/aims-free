// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/config/aimsdata_config.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/writer_d.h>
#include <aims/roi/hie.h>

using namespace std;
using namespace carto;
using namespace aims;

AIMS_INSTANTIATE_WRITER( Hierarchy);
