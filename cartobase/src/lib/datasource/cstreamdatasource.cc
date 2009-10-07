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

#include <cartobase/datasource/cstreamdatasource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace carto;
using namespace std;

CStreamDataSource::CStreamDataSource( FILE *f, int mode ) 
  : DataSource(), _f( f )
{
  _mode = mode;
}


CStreamDataSource::~CStreamDataSource()
{
  close();
}


DataSource* CStreamDataSource::clone() const
{
  return new CStreamDataSource( _f, _mode );
}


FILE *CStreamDataSource::filePtr() const
{
  return _f;
}


void CStreamDataSource::setFilePtr( FILE *f )
{
  _f = f;
}


int CStreamDataSource::iterateMode() const
{
  return DirectAccess | SequentialAccess;
}


bool CStreamDataSource::isOpen() const
{
  return _f != 0;
}


bool CStreamDataSource::open ( int mode )
{
  _mode = mode;
  if( _f != 0 )
    return _f;
  const char	*smode = "rb";
  switch( mode )
    {
    case Read:
      smode = "rb";
      break;
    case Write:
      smode = "wb";
      break;
    case ReadWrite:
      smode = "r+";
      break;
    }
  _f = fopen( url().c_str(), smode );
  return _f != 0;
}


void CStreamDataSource::close()
{
  if( _f )
    {
      fclose( _f );
      _f = 0;
    }
}


carto::offset_t CStreamDataSource::size() const
{
  struct stat	buf;
  if( fstat( fileno( _f ), &buf ) != 0 )
    return 0;
  return buf.st_size;
}


carto::offset_t CStreamDataSource::at() const
{
  if( _f )
    return (offset_t) ftell( _f );
  return 0;
}


bool CStreamDataSource::at( offset_t pos )
{
  if( _f )
    return fseek( _f, (long) pos, SEEK_SET ) != -1;
  return false;
}


bool CStreamDataSource::allowsMemoryMapping() const
{
  return true;
}


long CStreamDataSource::readBlock( char* data, unsigned long maxlen )
{
  return fread( data, 1, maxlen, _f );
}


long CStreamDataSource::writeBlock( const char* data, unsigned long len )
{
  return fwrite( data, 1, len, _f );
}


int CStreamDataSource::getch()
{
  char	c;
  if( readBlock( &c, 1 ) != 1 )
    return -1;
  return c;
}


int CStreamDataSource::putch( int ch )
{
  char	c = (char) ch;
  if( writeBlock( &c, 1 ) != 1 )
    return -1;
  return c;
}


bool CStreamDataSource::ungetch( int )
{
  return fseek( _f, -1, SEEK_CUR ) != -1;
}


