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

#include <cartobase/plugin/plugin.h>
#include <cartobase/object/pythonreader.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/config/verbose.h>
#include <cartobase/wip/io/minfXML2.h>
#include <iostream>
#ifndef CARTO_NO_DLOPEN
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#endif

using namespace carto;
using namespace std;


struct PluginManager::PluginManagerPrivate
{
  set<Plugin*>	plugins;
};


static PluginManager *& privatePluginManager()
{
  static PluginManager	*man = 0;
  return( man );
}


PluginManager::PluginManager() : d( new PluginManagerPrivate )
{
  delete privatePluginManager();
  privatePluginManager() = this;
}


PluginManager::~PluginManager()
{
  privatePluginManager() = 0;
  delete d;
}


PluginManager & PluginManager::singleton()
{
  PluginManager	*&man = privatePluginManager();
  if( !man )
    man = new PluginManager;
  return( *man );
}


void PluginManager::registerPlugin( Plugin *p )
{
  d->plugins.insert( p );
}


void PluginManager::unregisterPlugin( Plugin *p )
{
  d->plugins.erase( p );
}


set<Plugin *> PluginManager::plugins() const
{
  return( d->plugins );
}


Plugin::Plugin()
{
  PluginManager::singleton().registerPlugin( this );
}


Plugin::~Plugin()
{
  PluginManager::singleton().unregisterPlugin( this );
}


void Plugin::initialize()
{
}


string Plugin::name() const
{
  return( "unnamed plugin" );
}


// -----

PluginLoader::PluginFile::PluginFile( const string & fname,
                                      const string & ver )
  : filename( fname ), version( ver ), loaded( false )
{
}


list<PluginLoader::PluginFile> & PluginLoader::pluginFiles()
{
  static list<PluginFile>	plugins;
  return plugins;
}


#ifndef CARTO_NO_DLOPEN
static bool loadlib2( const string & lname, int verbose )
{
#ifdef _WIN32
  HANDLE	hl = GetModuleHandle( lname.c_str() );
  if( hl )
    {
      // already loaded
      CloseHandle( hl );
      //cout << "DLL " << lname << " alreay loaded\n";
      return true;
    }
  //cout << "loading DLL " << lname << endl;
  HMODULE	h = LoadLibrary( lname.c_str() );
#else

  static set<string>	libbase;

  if( libbase.find( lname ) != libbase.end() )
    return true;	// already loaded
  libbase.insert( lname );

  void	*h = dlopen( lname.c_str(), RTLD_GLOBAL|RTLD_NOW );

#endif

  if( !h )
    {
      if( verbose > 0 )
	{
	  cerr << lname << ": ";
#ifdef _WIN32
          const string	msg = string( "Could not load DLL " ) + lname;
          const char *er = msg.c_str();
#else
	  const char *er = dlerror();
#endif
	  if( er )
	    cerr << er << endl;
	  cerr << endl;
	}
      return false;
    }
  if( verbose > 1 )
    cout << "library " << lname << " successfully loaded.\n";

  return true;
}
#endif	// CARTO_NO_DLOPEN


static bool loadlib( const string & libname, const std::string & ver, 
                     int verbose )
{
#ifndef CARTO_NO_DLOPEN

  string	lname = libname;
  string	lnamespec;
  string::size_type	sopos = lname.rfind( ".so" );
  list<string>  liblist;
  list<string>  vers;
  list<string>::iterator iv, ev = vers.end();

  if( !ver.empty() )
  {
    // try first with full version
    vers.push_back( string( "." ) + ver );
    // then try to remove tiny version number
    string::size_type x = ver.rfind( '.' );
    if( x != string::npos )
      vers.push_back( string( "." ) + ver.substr( 0, x ) );
  }
  vers.push_back( "" ); // also try without version num

  if( !ver.empty() && sopos == lname.length() - 3 )
  {
#ifdef _WIN32
    lname.replace( sopos, 3, ".dll" );
#endif
    // lname += string( "." ) + ver;
    for( iv=vers.begin(); iv!=ev; ++iv )
      liblist.push_back( lname + *iv );
#ifdef __APPLE__
    lname = libname.substr( 0, sopos );
    for( iv=vers.begin(); iv!=ev; ++iv )
      liblist.push_back( lname + *iv + ".so" );
#endif
  }
#ifdef _WIN32
  else
  {
    sopos = lname.rfind( ".so." );
    if( sopos != string::npos )
    {
      lname.replace( sopos, 4, ".dll." );
    }
    liblist.push_back( lname );
  }
#endif

  list<string>::iterator il, el = liblist.end();
  for( il=liblist.begin(); il!=el; ++il )
    if( loadlib2( *il, verbose ) )
      return true;
  
  // else, not found
  return false;

#else	// CARTO_NO_DLOPEN

  return false;

#endif	// CARTO_NO_DLOPEN
}


namespace
{

  struct PluginLoader_PrivateStatic
  {
    PluginLoader_PrivateStatic() : loaded( false ) {}

    bool loaded;
  };

  PluginLoader_PrivateStatic & pluginLoaderStatic()
  {
    static PluginLoader_PrivateStatic ps;
    return ps;
  }

}


void PluginLoader::load( int verbose, bool forcereload )
{
/*  if( !forcereload && pluginLoaderStatic().loaded )
    return;*/
  if( !pluginLoaderStatic().loaded )
    // force initializinf libxml in main thread
    carto::init_libXML();
  pluginLoaderStatic().loaded = true;
  if( verbose < 0 )
    verbose = carto::debugMessageLevel;
  list<PluginFile>		& plugins = pluginFiles();
  list<PluginFile>::iterator	ip;
  for( ip=plugins.begin(); ip!=plugins.end(); ++ip )
    if( !ip->loaded || forcereload )
    {
      loadPluginFile( ip->filename, ip->version, verbose );
      ip->loaded = true;
    }
}


void PluginLoader::loadPluginFile( const std::string & p, 
                                   const std::string & ver, int verbose )
{
  // cout << "Plugin file: " << p << endl;

  ValueObject<Dictionary>	plugins;
  Object			modep;

  try
    {
      PythonReader	r( p );
      r.read( plugins );

      try
        {
          modep = plugins.getProperty( "release_plugins" );
        }
      catch( exception & )
        {
          // cerr << "no release_plugins\n";
          return;
        }

      if( modep )
        {
          ObjectVector		&dic = modep->value<ObjectVector>();
          ObjectVector::iterator	ip, ep = dic.end();
          for( ip=dic.begin(); ip!=ep; ++ip )
            try
              {
                const string & name = (*ip)->value<string>();
                loadlib( name, ver, verbose );
              }
            catch( exception & )
              {
              }
        }
    }
  catch( exception & )
    {
      // cerr << "-coulnd't read\n";
    }
}


