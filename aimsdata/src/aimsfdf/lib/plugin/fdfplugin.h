#ifndef AIMS_PLUGIN_FDFPLUGIN_H
#define AIMS_PLUGIN_FDFPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class FdfPlugin : public carto::Plugin
  {
  public:
    FdfPlugin();
    virtual ~FdfPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

