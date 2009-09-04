/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <cartobase/config/paths.h>
#include <cartobase/config/version.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <iostream>
#include <cstdlib>

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
      const char* shfj_path = getenv( "BRAINVISA_SHARE" );
      Directory	d( "/" );

      if( !shfj_path )
        shfj_path = getenv( "SHFJ_SHARED_PATH" );
      if( shfj_path )
        _shared = shfj_path;
      else
        {
          // look in PATH
          const char *path = getenv( "PATH" );
          if( path )
          {
            list<string> plist = FileUtil::filenamesSplit( path,
                string( "" ) + FileUtil::pathSeparator() );
            list<string>::const_iterator ip, ep = plist.end();
            for( ip=plist.begin(); ip!=ep; ++ip )
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
              d.chdir( p );
              if( d.isValid() )
              {
                _shared = p;
                break;
              }
            }
          }
        }
      if( _shared.empty() )
      {
        cerr << "no BRAINVISA_SHARE env variable!\n";
#ifdef _WIN32
        _shared = "C:\\appli\\shared-main";
#else
        _shared = "/home/appli/shared-main";
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
    }

  return _shfjShared;
}


