#ifndef CARTODATA_PLUGIN_TIFFPLUGIN_H
#define CARTODATA_PLUGIN_TIFFPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
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

