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

#ifndef CARTOBASE_STREAMDATASOURCE_DATASOURCE_H
#define CARTOBASE_STREAMDATASOURCE_DATASOURCE_H

#include <cartobase/datasource/datasource.h>
#include <cartobase/smart/rcptr.h>
#include <iostream>

namespace carto
{

  /// DataSource on a std::iostream (read/write stream)
  class StreamDataSource : public DataSource
  {
  public:
    /// Using an existing, external stream
    StreamDataSource( std::iostream &, 
                      const std::string & url = std::string() );
    /// Using a ref-counter to a stream
    StreamDataSource( rc_ptr<std::iostream>, 
                      const std::string & url = std::string() );
    virtual ~StreamDataSource();
    std::iostream & stream();

    virtual std::string url() const;
    virtual DataSource* clone() const;
    virtual bool open( int mode );
    virtual bool allowsMemoryMapping() const;
    virtual int iterateMode() const;
    virtual bool isOpen() const;
    virtual bool eof() const;
    virtual bool operator ! () const;
    virtual void close();
    /// always returns 0
    virtual offset_t size() const;
    virtual offset_t at() const;
    virtual bool at( offset_t pos );
    virtual long readBlock( char* data, unsigned long maxlen );
    virtual long writeBlock( const char* data, unsigned long len );
    virtual int getch();
    virtual int putch( int ch );
    virtual bool ungetch( int ch );
    bool isFile() const;

  private:
    std::iostream 		*_stream;
    rc_ptr<std::iostream>	_rcstr;
    std::string			_url;
  };


  /// DataSource on a std::istream (read-only stream)
  class IStreamDataSource : public DataSource
  {
  public:
    /// Using an existing, external stream
    IStreamDataSource( std::istream &, 
                       const std::string & url = std::string() );
    /// Using a ref-counter to a stream
    IStreamDataSource( rc_ptr<std::istream>, 
                       const std::string & url = std::string() );
    virtual ~IStreamDataSource();
    std::istream & stream();

    virtual std::string url() const;
    virtual DataSource* clone() const;
    virtual bool open( int mode );
    virtual bool allowsMemoryMapping() const;
    virtual int iterateMode() const;
    virtual bool isOpen() const;
    virtual bool eof() const;
    virtual bool operator ! () const;
    virtual void close();
    /// always returns 0
    virtual offset_t size() const;
    virtual offset_t at() const;
    virtual bool at( offset_t pos );
    virtual long readBlock( char* data, unsigned long maxlen );
    /// always fails
    virtual long writeBlock( const char* data, unsigned long len );
    virtual int getch();
    /// always fails
    virtual int putch( int ch );
    virtual bool ungetch( int ch );
    bool isFile() const;

  private:
    std::istream 		*_stream;
    rc_ptr<std::istream>	_rcstr;
    std::string			_url;
  };


  /// DataSource on a std::ostream (write-only stream)
  class OStreamDataSource : public DataSource
  {
  public:
    /// Using an existing, external stream
    OStreamDataSource( std::ostream &, 
                       const std::string & url = std::string() );
    /// Using a ref-counter to a stream
    OStreamDataSource( rc_ptr<std::ostream>, 
                       const std::string & url = std::string() );
    virtual ~OStreamDataSource();
    std::ostream & stream();

    virtual std::string url() const;
    virtual DataSource* clone() const;
    virtual bool open( int mode );
    virtual bool allowsMemoryMapping() const;
    virtual int iterateMode() const;
    virtual bool isOpen() const;
    virtual bool eof() const;
    virtual bool operator ! () const;
    virtual void close();
    /// always returns 0
    virtual offset_t size() const;
    virtual offset_t at() const;
    virtual bool at( offset_t pos );
    /// always fails
    virtual long readBlock( char* data, unsigned long maxlen );
    virtual long writeBlock( const char* data, unsigned long len );
    /// always fails
    virtual int getch();
    virtual int putch( int ch );
    /// always fails
    virtual bool ungetch( int ch );
    bool isFile() const;

  private:
    std::ostream 		*_stream;
    rc_ptr<std::ostream>	_rcstr;
    std::string			_url;
  };

}

#endif

