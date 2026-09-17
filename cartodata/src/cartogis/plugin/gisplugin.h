#ifndef CARTODATA_PLUGIN_GISPLUGIN_H
#define CARTODATA_PLUGIN_GISPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
{

  class GisPlugin : public carto::Plugin
  {
  public:
    GisPlugin();
    virtual ~GisPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

