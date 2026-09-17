#ifndef AIMS_PLUGIN_VIDAPLUGIN_H
#define AIMS_PLUGIN_VIDAPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class VidaPlugin : public carto::Plugin
  {
  public:
    VidaPlugin();
    virtual ~VidaPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

