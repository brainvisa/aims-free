#include <aims/plugin/jpegplugin.h>

using namespace aims;

namespace
{
  // force using libaimsjpeg.dylib on Mac
  bool __jpegplugin = JpegPlugin::noop();
}


