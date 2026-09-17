#include <cartodata/config/config.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/config/paths.h>
#include <list>
#include <string>

using namespace carto;
using namespace std;

namespace
{

  bool addCartodataPlugins()
  {
    list<PluginLoader::PluginFile> & plugins = PluginLoader::pluginFiles();
    string cartoplugins = Paths::findResourceFile( "plugins/cartodata.plugins",
                                                   "aims" );
    if( !cartoplugins.empty() )
    {
      plugins.push_back( PluginLoader::PluginFile( cartoplugins,
                                                   CARTODATA_VERSION ) );
      return true;
    }
    else
    {
      return false;
    }
  }

  bool dummy __attribute((unused)) = addCartodataPlugins();

}
