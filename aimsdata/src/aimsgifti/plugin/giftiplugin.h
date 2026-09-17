#ifndef AIMS_PLUGIN_GIFTIPLUGIN_H
#define AIMS_PLUGIN_GIFTIPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class GiftiPlugin : public carto::Plugin
  {
  public:
    GiftiPlugin();
    virtual ~GiftiPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

