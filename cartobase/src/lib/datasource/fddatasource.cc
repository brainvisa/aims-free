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

#include <cartobase/datasource/fddatasource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace carto;
using namespace std;

FDDataSource::FDDataSource( int fd, int mode ) 
  : DataSource(), _fd( fd )
{
  _mode = mode;
}


FDDataSource::~FDDataSource()
{
  close();
}


DataSource* FDDataSource::clone() const
{
  return new FDDataSource( _fd, _mode );
}


int FDDataSource::descriptor() const
{
  return _fd;
}


void FDDataSource::setDescriptor( int fd )
{
  _fd = fd;
}


int FDDataSource::iterateMode() const
{
  if( isFile() )
    return DirectAccess | SequentialAccess;
  return SequentialAccess;
}


bool FDDataSource::isOpen() const
{
  return _fd >= 0;
}


bool FDDataSource::open ( int mode )
{
  _mode = mode;
  return _fd >= 0;
}


void FDDataSource::close()
{
  if( _fd >= 0 )
    {
      ::close( _fd );
      _fd = -1;
    }
}


carto::offset_t FDDataSource::size() const
{
  // this test should rather be implemented somewhere in FileUtil
  /*
//#ifdef _WIN32
  // TODO
  return false;
//#else
  */

  /* test if _fd is actually a file, then mmap is allowed.
     Otherwise it is not allowed */
  struct stat	buf;
  if( fstat( _fd, &buf ) != 0 )
    return 0;
  return buf.st_size;
  //#endif
}


carto::offset_t FDDataSource::at() const
{
  if( _fd >= 0 )
    return lseek( _fd, 0, SEEK_CUR );
  return 0;
}


bool FDDataSource::at( offset_t pos )
{
  if( _fd >= 0 )
    return lseek( _fd, pos, SEEK_SET ) != -1;
  return false;
}


bool FDDataSource::allowsMemoryMapping() const
{
  /* for now mmap is allowed only on plain files (with filename) because
     - the mmap allocator needs to be rewritten
     - on windows, "file mapping" may require a filename
   */
  // return isFile();
  return false;
}


long FDDataSource::readBlock( char* data, unsigned long maxlen )
{
  return ::read( _fd, data, maxlen );
}


long FDDataSource::writeBlock( const char* data, unsigned long len )
{
  return ::write( _fd, data, len );
}


int FDDataSource::getch()
{
  char	c;
  if( readBlock( &c, 1 ) != 1 )
    return -1;
  return c;
}


int FDDataSource::putch( int ch )
{
  char	c = (char) ch;
  if( writeBlock( &c, 1 ) != 1 )
    return -1;
  return c;
}


bool FDDataSource::ungetch( int )
{
  return lseek( _fd, -1, SEEK_CUR ) != -1;
}


bool FDDataSource::isFile() const
{
  struct stat	buf;
  if( fstat( _fd, &buf ) != 0 )
    return false;
  return S_ISREG( buf.st_mode );
}


