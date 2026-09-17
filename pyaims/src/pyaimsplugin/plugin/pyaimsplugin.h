#ifndef AIMS_PYAIMSMODULE_PYAIMSMODULE_H
#define AIMS_PYAIMSMODULE_PYAIMSMODULE_H

#include <cartobase/plugin/plugin.h>

namespace aims
{

  class PyaimsPlugin : public carto::Plugin
  {
  public:
    PyaimsPlugin();
    virtual ~PyaimsPlugin();
    virtual std::string name() const;
    static bool noop();
  };

}

#endif

