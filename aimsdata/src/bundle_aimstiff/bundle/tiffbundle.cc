#include <aims/plugin/tiffplugin.h>

using namespace aims;

namespace
{
  // force using libaimstiff.dylib on Mac
  bool __tiffplugin = TiffPlugin::noop();
}

