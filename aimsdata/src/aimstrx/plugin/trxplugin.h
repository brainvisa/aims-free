
#ifndef AIMS_PLUGIN_TRXPLUGIN_H
#define AIMS_PLUGIN_TRXPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class TrxPlugin : public carto::Plugin
  {
  public:
    TrxPlugin();
    virtual ~TrxPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

