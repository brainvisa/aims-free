#ifndef CARTODATA_PLUGIN_OSPLUGIN_H
#define CARTODATA_PLUGIN_OSPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
{

  class OSPlugin : public carto::Plugin
  {
  public:
    OSPlugin();
    virtual ~OSPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

