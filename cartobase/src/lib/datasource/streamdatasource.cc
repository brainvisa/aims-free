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

#include <cartobase/datasource/streamdatasource.h>
#include <fstream>

using namespace carto;
using namespace std;

StreamDataSource::StreamDataSource( iostream & stream, const string & url )
  : DataSource(), _stream( &stream ), _url( url )
{
  _mode = ReadWrite;
}


StreamDataSource::StreamDataSource( rc_ptr<iostream> stream, 
                                    const string & url )
  : DataSource(), _stream( stream.get() ), _rcstr( stream ), _url( url )
{
  _mode = ReadWrite;
}


StreamDataSource::~StreamDataSource()
{
}


DataSource* StreamDataSource::clone() const
{
  if( _rcstr.get() )
    return new StreamDataSource( _rcstr, _url );
  return new StreamDataSource( *_stream, _url );
}


string StreamDataSource::url() const
{
  return _url;
}


bool StreamDataSource::open( int mode )
{
  fstream	*io = dynamic_cast<fstream *>( _stream );
  if( !io )
    return false;	// cannot open non-file streams

  _mode = mode;
  fstream::openmode	flags;
  switch( mode )
    {
    case Read:
      flags = ios::in | ios::binary;
      break;
    case Write:
      flags = ios::out | ios::binary;
      break;
    case ReadWrite:
      flags = ios::in | ios::out | ios::binary;
      break;
    default:
      flags = ios::in | ios::binary;
    }

  io->open( url().c_str(), flags );
  if( !*io )
    return false;

  io->unsetf( ios::skipws );

  return true;
}


bool StreamDataSource::allowsMemoryMapping() const
{
  return false;
}


int StreamDataSource::iterateMode() const
{
  if( isFile() )
    return DirectAccess | SequentialAccess;
  return SequentialAccess;
}


bool StreamDataSource::isOpen() const
{
  return _stream && *_stream;
}


bool StreamDataSource::eof() const
{
  return !_stream || _stream->eof();
}


bool StreamDataSource::operator ! () const
{
  return !_stream || !*_stream;
}


void StreamDataSource::close()
{
  fstream	*io = dynamic_cast<fstream *>( _stream );
  if( io )
    io->close();
}


carto::offset_t StreamDataSource::size() const
{
  return 0;
}


carto::offset_t StreamDataSource::at() const
{
  if( mode() == Write )
    return _stream->tellp();
  return _stream->tellg();
}


bool StreamDataSource::at( offset_t pos )
{
  if( mode() & Write && _stream != &cout )
    _stream->seekp( pos, ios::beg );
  if( mode() & Read && _stream != &cin )
    _stream->seekg( pos, ios::beg );
  return _stream->good();
}


long StreamDataSource::readBlock( char* data, unsigned long maxlen )
{
  istream::pos_type	pos = _stream->tellg();
  _stream->read( data, maxlen );
  return _stream->tellg() - pos;
}


long StreamDataSource::writeBlock( const char* data, unsigned long len )
{
  istream::pos_type	pos = _stream->tellp();
  _stream->write( data, len );
  return _stream->tellp() - pos;
}


int StreamDataSource::getch()
{
  return _stream->get();
}


int StreamDataSource::putch( int ch )
{
  _stream->put( ch );
  return ch;
}


bool StreamDataSource::ungetch( int ch )
{
  _stream->putback( ch );
  return _stream->good();
}


bool StreamDataSource::isFile() const
{
  return dynamic_cast<fstream *>( _stream );
}


iostream & StreamDataSource::stream()
{
  return *_stream;
}

// ----

IStreamDataSource::IStreamDataSource( istream & stream, const string & url )
  : DataSource(), _stream( &stream ), _url( url )
{
  _mode = Read;
}


IStreamDataSource::IStreamDataSource( rc_ptr<istream> stream, 
                                      const string & url )
  : DataSource(), _stream( stream.get() ), _rcstr( stream ), _url( url )
{
  _mode = Read;
}


IStreamDataSource::~IStreamDataSource()
{
}


DataSource* IStreamDataSource::clone() const
{
  if( _rcstr.get() )
    return new IStreamDataSource( _rcstr, _url );
  return new IStreamDataSource( *_stream, _url );
}


string IStreamDataSource::url() const
{
  return _url;
}


bool IStreamDataSource::open( int )
{
  ifstream	*io = dynamic_cast<ifstream *>( _stream );
  if( !io )
    return false;	// cannot open non-file streams

  _mode = Read;
  io->open( url().c_str(), ios::in | ios::binary );
  if( !*io )
    return false;

  io->unsetf( ios::skipws );

  return true;
}


bool IStreamDataSource::allowsMemoryMapping() const
{
  return false;
}


int IStreamDataSource::iterateMode() const
{
  if( isFile() )
    return DirectAccess | SequentialAccess;
  return SequentialAccess;
}


bool IStreamDataSource::isOpen() const
{
  return _stream && *_stream;
}


bool IStreamDataSource::eof() const
{
  return !_stream || _stream->eof();
}


bool IStreamDataSource::operator ! () const
{
  if( !_stream )
    return true;
  return !_stream || !*_stream;
}


void IStreamDataSource::close()
{
  if( !_stream )
    return;
  ifstream	*io = dynamic_cast<ifstream *>( _stream );
  if( io )
    io->close();
}


carto::offset_t IStreamDataSource::size() const
{
  return 0;
}


carto::offset_t IStreamDataSource::at() const
{
  return _stream->tellg();
}


bool IStreamDataSource::at( offset_t pos )
{
  if( _stream != &cin )
    _stream->seekg( pos, ios::beg );
  return _stream->good();
}


long IStreamDataSource::readBlock( char* data, unsigned long maxlen )
{
  istream::pos_type	pos = _stream->tellg();
  _stream->read( data, maxlen );
  return _stream->tellg() - pos;
}


long IStreamDataSource::writeBlock( const char*, unsigned long )
{
  return 0;
}


int IStreamDataSource::getch()
{
  return _stream->get();
}


int IStreamDataSource::putch( int )
{
  return 0;
}


bool IStreamDataSource::ungetch( int ch )
{
  _stream->putback( ch );
  return _stream->good();
}


bool IStreamDataSource::isFile() const
{
  return dynamic_cast<ifstream *>( _stream );
}


istream & IStreamDataSource::stream()
{
  return *_stream;
}


// ------

OStreamDataSource::OStreamDataSource( ostream & stream, const string & url )
  : DataSource(), _stream( &stream ), _url( url )
{
  _mode = Write;
}


OStreamDataSource::OStreamDataSource( rc_ptr<ostream> stream, 
                                      const string & url )
  : DataSource(), _stream( stream.get() ), _rcstr( stream ), _url( url )
{
  _mode = Write;
}


OStreamDataSource::~OStreamDataSource()
{
}


DataSource* OStreamDataSource::clone() const
{
  if( _rcstr.get() )
    return new OStreamDataSource( _rcstr, _url );
  return new OStreamDataSource( *_stream, _url );
}


string OStreamDataSource::url() const
{
  return _url;
}


bool OStreamDataSource::open( int )
{
  ofstream	*io = dynamic_cast<ofstream *>( _stream );
  if( !io )
    return false;	// cannot open non-file streams

  _mode = Write;
  io->open( url().c_str(), ios::out | ios::binary );
  if( !*io )
    return false;

  io->unsetf( ios::skipws );

  return true;
}


bool OStreamDataSource::allowsMemoryMapping() const
{
  return false;
}


int OStreamDataSource::iterateMode() const
{
  if( isFile() )
    return DirectAccess | SequentialAccess;
  return SequentialAccess;
}


bool OStreamDataSource::isOpen() const
{
  return _stream && *_stream;
}


bool OStreamDataSource::eof() const
{
  return !_stream || _stream->eof();
}


bool OStreamDataSource::operator ! () const
{
  return !_stream || !*_stream;
}


void OStreamDataSource::close()
{
  ofstream	*io = dynamic_cast<ofstream *>( _stream );
  if( io )
    io->close();
}


carto::offset_t OStreamDataSource::size() const
{
  return 0;
}


carto::offset_t OStreamDataSource::at() const
{
  return _stream->tellp();
}


bool OStreamDataSource::at( offset_t pos )
{
  if( _stream != &cout )
    _stream->seekp( pos, ios::beg );
  return _stream->good();
}


long OStreamDataSource::readBlock( char*, unsigned long )
{
  return 0;
}


long OStreamDataSource::writeBlock( const char* data, unsigned long len )
{
  istream::pos_type	pos = _stream->tellp();
  _stream->write( data, len );
  return _stream->tellp() - pos;
}


int OStreamDataSource::getch()
{
  return 0;
}


int OStreamDataSource::putch( int ch )
{
  _stream->put( ch );
  return ch;
}


bool OStreamDataSource::ungetch( int )
{
  return false;
}


bool OStreamDataSource::isFile() const
{
  return dynamic_cast<ofstream *>( _stream );
}


ostream & OStreamDataSource::stream()
{
  return *_stream;
}


