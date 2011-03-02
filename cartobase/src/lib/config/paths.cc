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

#include <cartobase/config/paths.h>
#include <cartobase/config/version.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <iostream>
#include <cstdlib>
#ifdef USE_SHARE_CONFIG
#include <brainvisa-share/config.h>
#endif
#ifdef USE_SOMA_CONFIG
#include <soma/config.h>
#endif
using namespace carto;
using namespace std;


const string & Paths::home()
{
  static string	_home;

  if( _home.empty() )
    {
      // Home directory
      char	s = FileUtil::separator();
      char*	home = getenv( "HOME" );
      if( home )
        _home = home;
      else
        {
          // on windows, we have to test many env variables
          home = getenv( "USERPROFILE" );
          if( home )
            _home = home;
          else
            {
              home = getenv( "HOMEPATH" );
              if( home )
                _home = home;
              else
                {
                  _home = s;	// use root directory
                  _home += '.'; //(and avoid double \ on windows)
                }

              char	*drive = getenv( "HOMEDRIVE" );
              if( !drive )
                {
                  drive = getenv( "SystemDrive" );
                  if( drive )
                    _home = string( drive ) + _home;
                  else
                    {
                      drive = getenv( "SystemRoot" );
                      if( !drive )
                        drive = getenv( "windir" );
                      if( drive )
                        {
                          string	x = drive;
                          if( x.length() >= 2 && x[1] == ':' )
                            _home = x.substr( 0, 2 ) + _home;
                        }
                    }
                }
            }
        }

    }

  return _home;
}


const string & Paths::memmap()
{
  return Paths::tempDir();
}


const string & Paths::tempDir()
{
  static string	_memmap;

  if( _memmap.empty() )
    {
      const char* mm_path = getenv( "AIMS_MM_PATH" );
      if ( mm_path )
        _memmap = mm_path;
      else
        {
          mm_path = getenv( "TEMP" );
          if( ! mm_path )
            {
              mm_path = getenv( "TMP" );
              if( !mm_path )
                {
#ifdef _WIN32
                  _memmap = "C:\\windows\\temp";
#else
                  _memmap = "/tmp";
#endif
                }
              else
                _memmap = mm_path;
            }
          else
            _memmap = mm_path;
        }
    }

  return _memmap;
}

const string & Paths::globalShared()
{
  static string	_shared;

  if( _shared.empty() )
  {
    list<string> plist;
    list<string> pbvshare;
    const char *env_path = getenv( "BRAINVISA_SHARE" );
    if( env_path )
      plist.push_back( env_path );
    env_path = getenv( "SHFJ_SHARED_PATH" );
    if( env_path )
      plist.push_back( env_path );
#ifdef USE_SOMA_CONFIG
    env_path = DEFAULT_BRAINVISA_SHARE;
    if( env_path )
      plist.push_back( env_path );
#endif

#ifdef USE_SHARE_CONFIG
    pbvshare.push_back( BRAINVISA_SHARE_DIRECTORY );
#else
    pbvshare.push_back( "brainvisa-share-" + cartobaseShortVersion() );
    pbvshare.push_back( "brainvisa-" + cartobaseShortVersion() );
#endif

    Directory d( "/" );
    list<string>::const_iterator ip, ep = plist.end();
    list<string>::const_iterator is, es = pbvshare.end();
    for( ip=plist.begin(); ip!=ep; ++ip )
    {
      for( is = pbvshare.begin(); is!=es; is++ )
      {
        string p;
        d.chdir( *ip + FileUtil::separator() + (*is) );
        if ( d.isValid() )
        {
          _shared = *ip;
          return _shared;
        }
      }
    }

    // look in argv[0] and PATH
    string search;
    if( !argv0().empty() )
    {
      if( !search.empty() )
        search += FileUtil::pathSeparator();
      search = FileUtil::dirname( argv0() );
    }
    const char *path = getenv( "PATH" );
    if( path )
    {
      if( !search.empty() )
        search += FileUtil::pathSeparator();
      search += path;
    }

    plist = FileUtil::filenamesSplit( search,
                                      string( "" ) + FileUtil::pathSeparator()
                                    );
    for( ip=plist.begin(), ep=plist.end(); ip!=ep; ++ip )
    {
      for( is = pbvshare.begin(); is!=es; is++ )
      {
        string p;
        if( ip->length() >= 4
            && ( ip->substr( ip->length()-4, 4 ) == "/bin"
#ifdef _WIN32
            || ip->substr( ip->length()-4, 4 ) == "\\bin"
#endif
          ) )
          p = ip->substr( 0, ip->length() - 4 );
        else if( ip->length() >= 19
                && ( ip->substr( ip->length()-19, 19 )
                  == "/bin/commands-links"
#ifdef _WIN32
                || ip->substr( ip->length()-19, 19 )
                  == "\\bin\\commands-links"
#endif
                ) )
          p = ip->substr( 0, ip->length() - 19 );
  
        if( p.empty() )
          p = "/";
        p += string( "" ) + FileUtil::separator() + "share";
        d.chdir( p + FileUtil::separator() + (*is) );
        if( d.isValid() )
        {
          _shared = p;
          ip = ep; // leave the ip loop
          --ip; // will be incremented anyway
          break;
        }
      }
    }

    if( _shared.empty() )
    {
      cerr << "no BRAINVISA_SHARE env variable!\n";
#ifdef _WIN32
      _shared = "C:\\brainvisa\\share";
#else
      _shared = "/usr/local/share";
      d.chdir( _shared );
      if( !d.isValid() )
      {
        _shared = "/usr/local/share";
        d.chdir( _shared );
        if( !d.isValid() )
          _shared = "/usr/share";
      }
#endif
    }
  }

  return _shared;
}


const string & Paths::shfjShared()
{
  static string	_shfjShared;

  if( _shfjShared.empty() )
    {
      char	s = FileUtil::separator();
      Directory	d( "/" );
      const string	& shared = Paths::globalShared();

#ifdef USE_SHARE_CONFIG
      _shfjShared = shared + s + BRAINVISA_SHARE_DIRECTORY;
#else
      _shfjShared = shared + s + "shfj-" + cartobaseShortVersion();
      d.chdir( _shfjShared );
      if( !d.isValid() )
        {
          _shfjShared = shared;
          _shfjShared = shared + s + "shfj";
          d.chdir( _shfjShared );
          if( !d.isValid() )
            _shfjShared = shared;
        }
#endif
    }

  return _shfjShared;
}


const list<string> & Paths::resourceSearchPath()
{
  static list<string> searchpath;
  if( searchpath.empty() )
  {
    char s = FileUtil::separator();
    searchpath.push_back( Paths::home() + s + ".brainvisa" );
    searchpath.push_back( Paths::home() );
    searchpath.push_back( Paths::globalShared() );
  }
  return searchpath;
}


list<string> Paths::findResourceFiles( const string & filename,
                                       const string & project,
                                       const string & ver )
{
  const list<string> & searchpath = resourceSearchPath();
  list<string> fpaths;
  char s = FileUtil::separator();
  list<string>::const_iterator ip, ep = searchpath.end();
  string ppath, proj;
  string version = ver;

  if( project.empty() )
#ifdef USE_SHARE_CONFIG
  {
    proj = BRAINVISA_SHARE_DIRECTORY;
    string::size_type x = proj.rfind( '-' ); // version
    if( x != string::npos )
    {
      if( version.empty() )
        version = proj.substr( x + 1, proj.length() - x - 1 );
      proj = proj.substr( 0, x );
    }
  }
#else
    proj = "shfj";
#endif
  else
    proj = project;
  if( !proj.empty() && !version.empty() )
    ppath = proj + "-" + version;
  else
    ppath = proj + "-" + cartobaseShortVersion();

  for( ip=searchpath.begin(); ip!=ep; ++ip )
  {
    // search full versionned path
    string fpath = *ip + s + ppath + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      fpaths.push_back( fpath );
    /* search full versionned path, hidden directpry
    (eg $HOME/.anatomist-4.1/...) */
    fpath = *ip + s + '.' + ppath + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      fpaths.push_back( fpath );
    // search in project without version
    fpath = *ip + s + proj + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      fpaths.push_back( fpath );
    // search in project without version, hidden directory
    fpath = *ip + s + '.' + proj + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      fpaths.push_back( fpath );
    // search in general share path
    fpath = *ip + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      fpaths.push_back( fpath );
  }

  return fpaths;
}


string Paths::findResourceFile( const string & filename,
                                const string & project,
                                const string & ver )
{
  const list<string> & searchpath = resourceSearchPath();
  char s = FileUtil::separator();
  list<string>::const_iterator ip, ep = searchpath.end();
  string ppath, proj;
  string version = ver;

  if( project.empty() )
#ifdef USE_SHARE_CONFIG
  {
    proj = BRAINVISA_SHARE_DIRECTORY;
    string::size_type x = proj.rfind( '-' ); // version
    if( x != string::npos )
    {
      if( version.empty() )
        version = proj.substr( x + 1, proj.length() - x - 1 );
      proj = proj.substr( 0, x );
    }
  }
#else
    proj = "shfj";
#endif
  else
    proj = project;
  if( !proj.empty() && !version.empty() )
    ppath = proj + "-" + version;
  else
    ppath = proj + "-" + cartobaseShortVersion();

  for( ip=searchpath.begin(); ip!=ep; ++ip )
  {
    // search full versionned path
    string fpath = *ip + s + ppath + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      return fpath;
    /* search full versionned path, hidden directpry
    (eg $HOME/.anatomist-4.1/...) */
    fpath = *ip + s + '.' + ppath + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      return fpath;
    // search in project without version
    fpath = *ip + s + proj + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      return fpath;
    // search in project without version, hidden directory
    fpath = *ip + s + '.' + proj + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      return fpath;
    // search in general share path
    fpath = *ip + s + filename;
    if( FileUtil::fileStat( fpath ).find( '+' ) != string::npos )
      return fpath;
  }

  return "";
}


namespace
{
  string & argv0_rw()
  {
    static string argv0;
    return argv0;
  }
}


const string & Paths::argv0()
{
  return argv0_rw();
}


void Paths::setArgv0( const string & argv0 )
{
  string & a0 = argv0_rw();
  a0 = argv0;
}


