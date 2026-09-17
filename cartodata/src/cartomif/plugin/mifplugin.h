#ifndef CARTODATA_PLUGIN_MIFPLUGIN_H
#define CARTODATA_PLUGIN_MIFPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
{

  class MifPlugin : public carto::Plugin
  {
  public:
    MifPlugin();
    virtual ~MifPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

