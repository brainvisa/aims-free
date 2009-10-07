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

#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/file.h>
#include <iostream>
#include <list>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

using namespace carto;
using namespace std;


namespace carto
{
  struct Directory::Private
  {
    Private();
    void scan() const;

    string			dirname;
    mutable bool		scanned;
    mutable set<string>	files;
    mutable set<string>	directories;
    mutable bool		state;
  };
}


Directory::Private::Private() : scanned( false ), state( false )
{
}


void Directory::Private::scan() const
{
  files.clear();
  directories.clear();
  scanned = true;
  state = false;

  // non-portable things
  DIR	*dir = opendir( dirname.c_str() );
  if( !dir ) {
    //cerr << "cannot read directory " << dirname.c_str() << endl;
    return;
  }
  state = true;

  struct dirent	*dent = readdir( dir );
#ifndef _DIRENT_HAVE_D_TYPE
  struct stat	s;
  char	sep = FileUtil::separator();
#endif

  while( dent )
    {
#ifdef _DIRENT_HAVE_D_TYPE	// valid under linux
      /* cout << "dir entry " << dent->d_name << ", type: " 
         << (int) dent->d_type << endl; */
      // filter out . and ..
      if( strcmp( dent->d_name, "." ) && strcmp( dent->d_name, ".." ) )
	switch( dent->d_type )
	  {
	  case DT_DIR:
	    directories.insert( dent->d_name );
	    //cout << "(dir)\n";
	    break;
	  case DT_FIFO:	// pipe
	  case DT_CHR:	// char device
	  case DT_BLK:	// block device
	  case DT_REG:	// regular file
	  case DT_LNK:	// link / symlink
	  case DT_SOCK:	// socket
#ifdef DT_WHT
	  case DT_WHT:	// (?)
#endif
	  default:
	    files.insert( dent->d_name );
	    //cout << "(file)\n";
	  }
#else	// solaris, ...
      // filter out . and ..
      // here we must stat files to get their type
      if( strcmp( dent->d_name, "." ) && strcmp( dent->d_name, ".." ) 
	  && !stat(  (dirname + sep + dent->d_name).c_str(), &s ) )
	switch( s.st_mode )
	  {
	  case S_IFDIR:
	    if( !strcmp( dent->d_name, "." ) || !strcmp( dent->d_name, ".." ) )
	      break;
	    directories.insert( dent->d_name );
	    break;
	  case S_IFIFO:	// pipe
	  case S_IFCHR:		// char device
	  case S_IFBLK:		// block device
	  case S_IFREG:		// regular file
#ifndef _WIN32
	  case S_IFLNK:		// symlink (not returned by stat() but lstat())
#endif
	  default:
	    files.insert( dent->d_name );
	  }
#endif
      dent = readdir( dir );
    }
  closedir( dir );
}


Directory::Directory( const string & dname )
  : d( new Directory::Private )
{
  d->dirname = dname;
}


Directory::~Directory()
{
  delete d;
}


string Directory::dirname() const
{
  return( d->dirname );
}


void Directory::chdir( const string & dname )
{
  d->dirname = dname;
  d->scanned = false;
  d->files.clear();
  d->directories.clear();
  d->state = false;
}


set<string> Directory::files() const
{
  if( !d->scanned )
    d->scan();
  return( d->files );
}


set<string> Directory::directories() const
{
  if( !d->scanned )
    d->scan();
  return( d->directories );
}


bool Directory::isValid() const
{
  // this is not optimal, we should not entirely scan the directory
  if( !d->scanned )
    d->scan();
  return( d->state );
}


void Directory::mkdir() const
{
  DIR	*dir = opendir( dirname().c_str() );
  if( !dir )
    {
      d->state = true;
      d->scanned = false;
#if defined( _WIN32 )
      if( ::mkdir( d->dirname.c_str() ) )
#else
      if( ::mkdir( d->dirname.c_str(),
            S_IRWXU | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH
                | S_IXOTH ) )
#endif
        throw file_error( "couldn't create directory", d->dirname );
    }
  else
    closedir( dir );
}


void Directory::makedirs() const
{
  string	dn = dirname();
  list<string>	dirs;
  Directory	dir( FileUtil::dirname( dirname() ) );

  while( !dir.isValid() && dir.dirname() != dn )
    {
      dn = dir.dirname();
      dirs.push_back( dn );
      dir.chdir( FileUtil::dirname( dn ) );
    }
  dirs.push_back( dirname() );

  list<string>::iterator	i, e = dirs.end();
  for( i=dirs.begin(); i!=e; ++i )
    {
      dir.chdir( *i );
      dir.mkdir();
    }
}


