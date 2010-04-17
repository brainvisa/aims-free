namespace carto
{

  class PluginLoader
  {
%TypeHeaderCode
#include <cartobase/plugin/plugin.h>
%End

  public:
    struct PluginFile
    {
      PluginFile( const std::string &file, const std::string &ver="" );
      std::string filename;
      std::string version;
      bool loaded;
    };

    static void load( int verboseLevel=-1, bool forcereload=false );
    static void loadPluginFile( const std::string &file,
                                const std::string &version="",
                                int verboseLevel=0 );
    static list_PluginFile & pluginFiles();
  };

};
