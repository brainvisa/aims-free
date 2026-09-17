#ifndef CARTODATA_PLUGIN_MINCPLUGIN_H
#define CARTODATA_PLUGIN_MINCPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
{

  class MincPlugin : public carto::Plugin
  {
  public:
    MincPlugin();
    virtual ~MincPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

