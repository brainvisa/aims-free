#ifndef CARTODATA_PLUGIN_NIFTIPLUGIN_H
#define CARTODATA_PLUGIN_NIFTIPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
{

  class NiftiPlugin : public carto::Plugin
  {
  public:
    NiftiPlugin();
    virtual ~NiftiPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

