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

#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <cartobase/config/paths.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace carto;
using namespace std;


char FileUtil::separator()
{
#ifdef _WIN32
  return '\\';
#else
  return '/';
#endif
}


char FileUtil::pathSeparator()
{
#ifdef _WIN32
  return ';';
#else
  return ':';
#endif
}


string FileUtil::basename( const string & name )
{
  char			sep = separator();
  string::size_type	pos1 = name.rfind( sep );
#ifdef _WIN32
  string::size_type	pos2 = name.rfind( '/' );
  if( pos2 != string::npos && ( pos1 == string::npos || pos2 > pos1 ) )
      pos1 = pos2;
#endif
  if( pos1 == string::npos )
    if( name == "." || name == ".." )
      return "";
    else
      pos1 = 0;
  else
    ++pos1;
  return name.substr( pos1, name.length() - pos1 );
}


string FileUtil::dirname( const string & name )
{
  char			sep = separator();
  string::size_type	pos1 = name.rfind( sep );
#ifdef _WIN32
  string::size_type	pos2 = name.rfind( '/' );
  if( pos2 != string::npos && ( pos1 == string::npos || pos2 > pos1 ) )
      pos1 = pos2;
#endif
  if( pos1 == string::npos )
  {
    if( name == ".." )
      return "..";
    else
      return ".";
  }
  if( pos1 == 0 )
    {
      string	s( "/" );
      s[0] = sep;
      return s;
    }
  return name.substr( 0, pos1 );
}


string FileUtil::fileStat( const string & name )
{
  struct stat	s;

  if( stat( name.c_str(), &s ) )
    return "";

  string	code = "+";
  if( S_ISDIR( s.st_mode ) )
    code += 'd';
  if( s.st_mode & S_IRUSR )
    code += 'r';
  if( s.st_mode & S_IWUSR )
    code += 'w';
  if( s.st_mode & S_IXUSR )
    code += 'x';

  return code;
}


string FileUtil::extension( const std::string & name )
{
  string::size_type	epos = name.rfind( '.' );
  if( epos == string::npos )
    return "";

  char			sep = separator();
  string::size_type	pos1 = name.rfind( sep );
#ifdef _WIN32
  string::size_type	pos2 = name.rfind( '/' );
  if( pos2 != string::npos && ( pos1 == string::npos || pos2 > pos1 ) )
      pos1 = pos2;
#endif
  if( pos1 == string::npos || pos1 < epos )
    return name.substr( epos + 1, name.length() - epos - 1 );
  return "";
}


string FileUtil::removeExtension( const std::string & name )
{
  string::size_type	epos = name.rfind( '.' );
  if( epos == string::npos )
    return name;

  char			sep = separator();
  string::size_type	pos1 = name.rfind( sep );
#ifdef _WIN32
  string::size_type	pos2 = name.rfind( '/' );
  if( pos2 != string::npos && ( pos1 == string::npos || pos2 > pos1 ) )
      pos1 = pos2;
#endif
  if( pos1 == string::npos || pos1 < epos )
    return name.substr( 0, epos );
  return name;
}


bool FileUtil::isAbsPath( const string & path )
{
#ifdef _WIN32
  if( path.empty() )
    return false;
  if( path[0] == '/' or path[0] == '\\' )
    return true;
  char	c = path[0];
  return path.length() >= 3 and path[1] == ':' 
    and ( path[2] == '/' or path[2] == '\\' ) 
    and ( ( c >= 'A' and c <= 'Z' ) or ( c >= 'a' and c <= 'z' ) );
#else
  return !path.empty() && path[0] == '/';
#endif
}


string FileUtil::temporaryFile( const string & prefix, int & fd )
{
  string	fname;

  string dir = dirname( prefix );
  if( dir.empty() || dir == "." )
    {
      fname = Paths::tempDir() + separator() + removeExtension( prefix );
      if( prefix.empty() )
        fname += "carto"; // default base name
    }
  else
    fname = removeExtension( prefix );
  string	ext = extension( prefix );

#ifdef _WIN32

  char tempname[MAX_PATH];
  GetTempFileName( dirname( fname ).c_str(), basename( fname ).c_str(), 0, 
                   tempname );
  fname = tempname;
  if( !ext.empty() )
    {
      unlink( tempname );
      fname += string( "." ) + ext;
    }
  fd = ::open( fname.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777 );
  if( fd != -1 )
    unlink( fname.c_str() );

#else

  fname += "XXXXXX";

  fd = mkstemp( (char *) fname.c_str() );
  if( fd != -1 )
    {
      // unlink file to ensure it will be deleted after usage
      unlink( fname.c_str() );
      if( !ext.empty() )
        {
          ::close( fd );
          fname += string( "." ) + ext;
#ifdef __APPLE__
          // MacOS 10 doesn't have O_LARGEFILE flag
          int	mode = O_RDWR | O_CREAT | O_TRUNC;
#else
          int	mode = O_RDWR | O_CREAT | O_TRUNC | O_LARGEFILE;
#endif
          fd = ::open( fname.c_str(), mode, 0777 );
          if( fd != -1 )
            unlink( fname.c_str() );
        }
    }

#endif

  if( fd == -1 )
    {
      cerr << "unable to create a temporary file of base " << fname 
           << " !" << endl;
      return "";
    }

  return fname;
}


string FileUtil::temporaryDirectory( const string & prefix )
{
  string	fname;

  if( dirname( prefix ).empty() )
    {
      fname = Paths::tempDir() + separator() + prefix;
      if( prefix.empty() )
        fname += "carto"; // default base name
    }
  else
    fname = prefix;

#ifdef _WIN32

  char tempname[MAX_PATH];
  GetTempFileName( dirname( fname ).c_str(), basename( fname ).c_str(), 0, 
                   tempname );
  unlink( tempname );
  fname = tempname;
  Directory	d( fname );
  d.mkdir();

#else

  fname += "XXXXXX";

  int	fd = mkstemp( (char *) fname.c_str() );
  if( fd != -1 )
    {
      // unlink file to ensure it will be deleted after usage
      unlink( fname.c_str() );
      ::close( fd );
      Directory	d( fname );
      d.mkdir();
    }

#endif

  return fname;
}


list<string> FileUtil::filenamesSplit( const string & fnames,
                                       const string & sep )
{
  char  quotec = '\0', c;
  bool  quote = false;
  bool  esc = false;
  string::size_type sepcnt = 0, lsep = sep.length();
  list<string>  names;
  string::size_type pos, n = fnames.length();
  string  cur, curs;

  for( pos=0; pos!=n; ++pos )
  {
    c = fnames[pos];
    if( esc )
      cur += c;
    else
    {
      if( c == '\\' )
        esc = true;
      else if( ( quote && c == quotec )
                 || ( !quote && ( c == '"' || c == '\'' ) ) )
      {
        quotec = c;
        quote = !quote;
        sepcnt = 0;
      }
      else if( !quote && c == sep[sepcnt] )
      {
        ++sepcnt;
        curs += c;
        if( sepcnt == lsep )
        {
          names.push_back( cur );
          sepcnt = 0;
          curs.clear();
          cur.clear();
        }
      }
      else
      {
        cur += curs;
        curs.clear();
        sepcnt = 0;
        cur += c;
      }
    }
  }
  cur += curs;
  if( !cur.empty() )
  {
    names.push_back( cur );
  }

  return names;
}

