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

#include <cartobase/datasource/bufferdatasource.h>
#include <cstring>

using namespace carto;
using namespace std;

BufferDataSource::BufferDataSource( char* ptr, unsigned long size, int mode )
  : _buffer( ptr ), _size( size ), _pos( 0 )
{
  _mode = mode;
}


BufferDataSource::~BufferDataSource()
{
}


DataSource* BufferDataSource::clone() const
{
  return new BufferDataSource( _buffer, _size, _mode );
}


int BufferDataSource::iterateMode() const
{
  return DirectAccess | SequentialAccess;
}


bool BufferDataSource::isOpen() const
{
  return _buffer != 0;
}


bool BufferDataSource::open( int mode )
{
  _mode = mode;
  _pos = 0;
  return _buffer != 0;
}


void BufferDataSource::setBuffer( char* ptr, unsigned long size, int mode )
{
  _buffer = ptr;
  _size = size;
  _mode = mode;
  _pos = 0;
}


void BufferDataSource::close()
{
  _pos = 0;
}


carto::offset_t BufferDataSource::size() const
{
  return _size;
}


carto::offset_t BufferDataSource::at() const
{
  return _pos;
}


bool BufferDataSource::at( offset_t pos )
{
  if( pos < _size )
    {
      _pos = pos;
      return true;
    }
  return false;
}


long BufferDataSource::readBlock( char * data, unsigned long maxlen )
{
  if( !(mode() & Read) || _pos >= _size )
    return -1;
  if( _size - _pos > maxlen )
    maxlen = _size - _pos;
  memcpy( data, _buffer + _pos, maxlen );
  _pos += maxlen;
  return (long) maxlen;
}


long BufferDataSource::writeBlock( const char * data, unsigned long len )
{
  if( !(mode() & Write) || _pos >= _size )
    return -1;
  if( _size - _pos > len )
    len = _size - _pos;
  memcpy( _buffer + _pos, data, len );
  _pos += len;
  return (long) len;
}


int BufferDataSource::getch()
{
  if( !(mode() & Read) || _pos >= _size )
    return -1;
  char	c = *(_buffer + _pos);
  ++_pos;
  return c;
}


int BufferDataSource::putch( int ch )
{
  if( !(mode() & Write) || _pos >= _size )
    return -1;
  char	c = (char) ch;
  *(_buffer + _pos) = c;
  ++_pos;
  return ch;
}


bool BufferDataSource::ungetch( int ch )
{
  if( !(mode() & Read) || _pos == 0 )
    return false;
  --_pos;
  _buffer[ _pos ] = ch;
  return true;
}


bool BufferDataSource::allowsMemoryMapping() const
{
  return false;
}


const char* BufferDataSource::buffer() const
{
  return _buffer;
}


char* BufferDataSource::buffer()
{
  return _buffer;
}


