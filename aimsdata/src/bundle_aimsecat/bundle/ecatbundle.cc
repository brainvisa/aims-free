#include <aims/plugin/ecatplugin.h>

using namespace aims;

namespace
{
  // force using libaimsecat.dylib on Mac
  bool __ecatplugin = EcatPlugin::noop();
}


