#ifndef AIMS_PLUGIN_JPEGPLUGIN_H
#define AIMS_PLUGIN_JPEGPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class JpegPlugin : public carto::Plugin
  {
  public:
    JpegPlugin();
    virtual ~JpegPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

