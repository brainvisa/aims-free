#include <aims/plugin/vidaplugin.h>

using namespace aims;

namespace
{
  // force using libaimsvida.dylib on Mac
  bool __vidaplugin = VidaPlugin::noop();
}


