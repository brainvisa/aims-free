
%ModuleHeaderCode
#include <cartobase/config/paths.h>
%End

namespace carto
{

  namespace Paths
  {
    const std::string & home();
    const std::string & tempDir();
    const std::string & memmap();
    const std::string & globalShared();
    const std::string & shfjShared();
  };

};

