/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#ifndef CARTOBASE_PLUGIN_PLUGIN_H
#define CARTOBASE_PLUGIN_PLUGIN_H

#include <string>
#include <set>
#include <list>

namespace carto
{

  class Plugin
  {
    friend class PluginManager;

  public:
    Plugin();
    virtual ~Plugin();
    virtual std::string name() const;

  private:
    virtual void initialize();
  };

  class PluginManager
  {
  public:
    PluginManager();
    ~PluginManager();
    void registerPlugin( Plugin* p );
    void unregisterPlugin( Plugin* p );
    std::set<Plugin*> plugins() const;
    static PluginManager & singleton();

  private:
    struct PluginManagerPrivate;
    PluginManagerPrivate	*d;
  };


  class PluginLoader
  {
  public:
    /// load the complete pluginFiles list
    static void load( int verboseLevel = -1, bool forcereload = false );
    ///	load a specific plugin file (in Python format)
    static void loadPluginFile( const std::string & file, 
                                const std::string & version = "", 
			        int verboseLevel = 0 );
    struct PluginFile
    {
      PluginFile( const std::string & file, const std::string & ver = "" );
      bool operator == ( const PluginFile & pf ) const
      { return filename == pf.filename && version == pf.version; }
      std::string filename;
      std::string version;
      bool loaded;
    };
    /** static list of plugin files to be loaded.
        A plugin file version is normally the version of the corresponding 
        library. It is only a hint: any version actually specified in 
        the plugin file itself overrides this version. If no version is 
        provided here, libs in the plugin file are loaded as is.
     */
    static std::list<PluginFile> & pluginFiles();
  };

}

#endif

