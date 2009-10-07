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

#ifndef CARTOBASE_DATASOURCE_DATASOURCE_H
#define CARTOBASE_DATASOURCE_DATASOURCE_H

#include <cartobase/smart/rcptr.h>
#include <string>

namespace carto
{

  /// Offsets are 64 bits if supported
#if defined(__osf__)
  typedef unsigned long offset_t;
#else
  typedef unsigned long long offset_t;
#endif

  /** Abstraction layer for various data sources (file, 
      buffer, socket...).

      It can be seen as a stream, and is inspired by the QIODevice of 
      Trolltech's Qt library (http://doc.trolltech.com/3.3/qiodevice.html)
  */
  class DataSource : public virtual RCObject
  {
  public:
    enum Mode
      {
        Read = 1, 
        Write = 2,
        ReadWrite = 3,
      };

    enum IterateMode
      {
        DirectAccess = 1, 
        SequentialAccess = 2,
      };

    DataSource();
    virtual ~DataSource();

    /// access mode(s) (read/write): bitwise OR of Mode values
    int mode() const;

    virtual DataSource* clone() const = 0;
    /// possible iteration mode(s): bitwise OR of IterateMode values
    virtual int iterateMode() const = 0;
    virtual bool isOpen() const = 0;
    virtual bool eof() const;
    virtual bool operator ! () const;
    virtual operator bool () const;
    virtual bool open ( int mode ) = 0;
    virtual void close() = 0;
    virtual void flush();
    virtual offset_t size() const = 0;
    virtual offset_t at() const = 0;
    virtual bool at( offset_t pos ) = 0;
    virtual bool atEnd() const;
    bool reset();
    virtual long readBlock( char * data, unsigned long maxlen ) = 0;
    virtual long writeBlock( const char * data, unsigned long len ) = 0;
    virtual int getch() = 0;
    virtual int putch( int ch ) = 0;
    virtual bool ungetch( int ch ) = 0;
    virtual bool allowsMemoryMapping() const = 0;
    virtual std::string url() const;

    /** An empty ref-counter that is more convenient than calling a constructor
        of rc_ptr<DataSource> (useful when calling functions)
     */
    static const rc_ptr<DataSource> none();

  protected:
    int		_mode;
  };

}

#endif

