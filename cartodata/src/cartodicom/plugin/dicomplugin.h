#ifndef CARTODATA_PLUGIN_DICOMPLUGIN_H
#define CARTODATA_PLUGIN_DICOMPLUGIN_H

#include <cartobase/plugin/plugin.h>

namespace cartodata
{

  class DicomPlugin : public carto::Plugin
  {
  public:
    DicomPlugin();
    virtual ~DicomPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

