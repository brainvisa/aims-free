#include <aims/plugin/qtformatsplugin.h>

using namespace aims;

namespace
{
  // force using libaimsqtformats.dylib on Mac
  bool __aimsqtplugin = QtFormatsPlugin::noop();
}


