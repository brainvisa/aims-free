#ifndef AIMS_PLUGIN_PLYPLUGIN_H
#define AIMS_PLUGIN_PLYPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class PlyPlugin : public carto::Plugin
  {
  public:
    PlyPlugin();
    virtual ~PlyPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

