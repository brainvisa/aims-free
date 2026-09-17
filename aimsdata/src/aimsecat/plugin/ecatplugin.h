#ifndef AIMS_PLUGIN_ECATPLUGIN_H
#define AIMS_PLUGIN_ECATPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class EcatPlugin : public carto::Plugin
  {
  public:
    EcatPlugin();
    virtual ~EcatPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

