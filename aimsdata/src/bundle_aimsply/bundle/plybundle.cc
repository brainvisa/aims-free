#include <aims/plugin/plyplugin.h>

using namespace aims;

namespace
{
  // force using libaimsply.dylib on Mac
  bool __plyplugin = PlyPlugin::noop();
}


