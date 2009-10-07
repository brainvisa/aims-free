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

#include <cartobase/datasource/filedatasource.h>
#include <cartobase/stream/fileutil.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace carto;
using namespace std;

FileDataSource::FileDataSource( const string & filename, offset_t offset, 
                                int mode )
  :
#ifdef CARTO_FILEDS_AS_FD
  FDDataSource( -1, mode ), 
#else
  CStreamDataSource( 0, mode ), 
#endif
  _url( filename ), _offset( offset )
{
  if( mode & Write )
    {
      string st = FileUtil::fileStat( filename );
      if( st.find( '+' ) != string::npos && st.find( 'w' ) == string::npos )
        {
          // file is not writeable
          mode &= ~Write;
          _mode = mode;
        }
    }
}


FileDataSource::~FileDataSource()
{
}


DataSource* FileDataSource::clone() const
{
  return new FileDataSource( _url, _offset, _mode );
}


string FileDataSource::url() const
{
  return _url;
}


carto::offset_t FileDataSource::initialOffset() const
{
  return _offset;
}


bool FileDataSource::open( int mode )
{
  if( isOpen() )
    return true;

#ifdef CARTO_FILEDS_AS_FD

  _mode = mode;
  int	flags;
  switch( mode )
    {
    case Read:
      flags = O_RDONLY;
      break;
    case Write:
      flags = O_WRONLY | O_CREAT | O_TRUNC;
      break;
    case ReadWrite:
      flags = O_RDWR;
      break;
    default:
      flags = O_RDONLY;
    }
#if !defined( __APPLE__ ) && !defined( _WIN32 )
  // MacOS 10 doesn't have this flag
  flags |= O_LARGEFILE;
#endif

#ifdef _WIN32
  int	fd = ::open( url().c_str(), flags );
#else
  int	fd = ::open( url().c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP 
                     | S_IWGRP | S_IROTH | S_IWOTH );
#endif

  setDescriptor( fd );
  if( _fd < 0 && ( mode & Write ) )
    {
      // try to reopen it read-only
      return open( mode & ~Write );
    }

  if( descriptor() >= 0 && _offset != 0 )
    lseek( _fd, _offset, SEEK_SET );
  return _fd >= 0;

#else // CARTO_FILEDS_AS_FD

  return CStreamDataSource::open( mode );

#endif
}


bool FileDataSource::allowsMemoryMapping() const
{
  return true;
}


carto::offset_t FileDataSource::size() const
{
#ifdef CARTO_FILEDS_AS_FD
  return (offset_t) ( FDDataSource::size() - _offset );
#else
  return (offset_t) ( CStreamDataSource::size() - _offset );
#endif
}


carto::offset_t FileDataSource::at() const
{
#ifdef CARTO_FILEDS_AS_FD
  return (offset_t) ( FDDataSource::at() - _offset );
#else
  return (offset_t) ( CStreamDataSource::at() - _offset );
#endif
}


bool FileDataSource::at( offset_t pos )
{
#ifdef CARTO_FILEDS_AS_FD
  return FDDataSource::at( pos + _offset );
#else
  return CStreamDataSource::at( pos + _offset );
#endif
}


