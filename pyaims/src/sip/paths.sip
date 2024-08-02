
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
    const std::string & installRoot();
    const std::string & globalShared();
    const std::string & shfjShared();
    const list_STRING & resourceSearchPath();
    list_STRING findResourceFiles( const std::string & filename,
                                   const std::string & project = "",
                                   const std::string & version = "" );
    std::string findResourceFile( const std::string & filename,
                                  const std::string & project = "",
                                  const std::string & version = "" );
  };

};

