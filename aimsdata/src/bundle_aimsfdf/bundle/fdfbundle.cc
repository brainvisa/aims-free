#include <aims/plugin/fdfplugin.h>

using namespace aims;

namespace
{
  // force using libaimsfdf.dylib on Mac
  bool __fdfplugin = FdfPlugin::noop();
}

