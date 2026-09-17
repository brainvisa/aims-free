#ifndef AIMS_PLUGIN_TIFFPLUGIN_H
#define AIMS_PLUGIN_TIFFPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class TiffPlugin : public carto::Plugin
  {
  public:
    TiffPlugin();
    virtual ~TiffPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

