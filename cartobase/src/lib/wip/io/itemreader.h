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

#ifndef CARTOBASE_IO_ITEMREADER_H
#define CARTOBASE_IO_ITEMREADER_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/datasource/datasource.h>
#include <cartobase/wip/io/asciidatasourcetraits.h>
#include <algorithm>

namespace carto
{

  /**	Low-level "small item" reader, used by higher-level file readers. 
	It can read either a single item, or an array of them. This 
	template class is inherited by specific item readers, esp. by 
	the DefaultItemReader classes.

	Usage: you generally have to allocate two readers: one as a factory 
	to use the correct item variant, and another one to switch on the 
	reading-mode specific ItemReader:

	\code
	#include <cartobase/io/defaultitemreader.h>
	using namespace carto;
	// Let's read a vector of signed 16 bits elements
	short				shortarray[ 100 ];
	DefaultItemReader<short>	ir1;
	bool				binary;
	bool				bswap;

	// get binary and bswap from the header of the file to read
	// ...
	// now we assume it's done

	ItemReader<short>	*ir = ir1.reader( binary, bswap );
	// we use ir in the following, we can forget ir1 now.

	FileDataSource	ds( "file.dat" );
	if( !ds.open( DataSource::Read ) )
	  return false; // fail
	// read 100 elements
	ir->read( ds, shortarray, 100 );
	// don't forget to destroy the reader after use
	// (or put it in a scoped_ptr)
	delete ir;
	\endcode
  */
  template<typename T>
  class ItemReader
  {
  public:
    virtual ~ItemReader() {}

    /// Factory function
    virtual ItemReader<T>* reader( bool binary = true, 
                                   bool bswap = false ) const = 0;

    /** Reading on an arbitrary DataSource
        \param ds source of data (file, buffer, socket...)
        \param pitem buffer to fill with read values
        \param n number of elements to read
        \return number of elements actually read
     */
    virtual long read( DataSource & ds, T* pitem, size_t n = 1 ) const = 0;
  };


  /**	Default low-levels readers.
	They are a default implementation of the readers for most common types,
	and can be specialized for other types if needed.
  */
  template <typename T>
  class DefaultItemReader : public ItemReader<T>
  {
  public:
    DefaultItemReader() : ItemReader<T>() { }
    virtual ~DefaultItemReader() { }

    virtual ItemReader<T>* reader( bool binary = true, 
                                   bool bswap = false ) const;

    virtual long read( DataSource & ds, T* pitem, size_t n = 1 ) const;
  };


  template <typename T>
  class DefaultAsciiItemReader : public DefaultItemReader<T>
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<T>() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual long read( DataSource & ds, T* pitem, size_t n = 1 ) const;
    bool readOne( DataSource & ds, T* pitem ) const;
  };


  template <typename T>
  class DefaultBSwapItemReader : public DefaultItemReader<T>
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<T>() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual long read( DataSource & ds, T* pitem, size_t n = 1 ) const;
  };


  // -----------

  template <typename T>
  inline
  ItemReader<T>* DefaultItemReader<T>::reader( bool binary, bool bswap ) const
  {
    if( !binary )
      return new DefaultAsciiItemReader<T>;
    if( bswap )
      return  new DefaultBSwapItemReader<T>;
    return new DefaultItemReader<T>;
  }


  template <>
  inline
  ItemReader<int8_t>* DefaultItemReader<int8_t>::reader( bool binary, 
                                                         bool ) const
  {
    if( !binary )
      return new DefaultAsciiItemReader<int8_t>;
    return new DefaultItemReader<int8_t>;
  }


  template <>
  inline
  ItemReader<uint8_t>* DefaultItemReader<uint8_t>::reader( bool binary, 
                                                           bool ) const
  {
    if( !binary )
      return new DefaultAsciiItemReader<uint8_t>;
    return new DefaultItemReader<uint8_t>;
  }


  template <typename T>
  inline
  long DefaultItemReader<T>::read( DataSource & ds, T* pitem, 
                                   size_t n ) const
  {
     // warning: alignment problems not taken into account here
    long	len = ds.readBlock( (char *) pitem, n * sizeof(T) );
    return len / sizeof(T);
  }


  template <typename T>
  inline
  long DefaultBSwapItemReader<T>::read( DataSource & ds, T* pitem, 
                                        size_t n ) const
  {
     // warning: alignment problems not taken into account here
    long	len = ds.readBlock( (char *) pitem, n * sizeof(T) );
    uint8_t	*ptr = (uint8_t*) pitem;
    for( long k=0; k<len; k+=sizeof(T) )
      for( size_t b=0; b<sizeof(T)/2; ++b )
	std::swap( ptr[k+b], ptr[k+sizeof(T)-1-b] );
    return len / sizeof(T);
  }


  template <typename T>
  inline
  long DefaultAsciiItemReader<T>::read( DataSource & ds, T* pitem, 
                                        size_t n ) const
  {
    size_t	i;
    int		c;
    T		*ptr = pitem;

    for( i=0; i<n; ++i, ++ptr )
      {
        // skip formatting chars
        do
          {
            c = ds.getch();
          }
        while( ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' ) 
               && ds.isOpen() );
        if( !ds.isOpen() )
          break;
        ds.ungetch( c );
        if( !readOne( ds, ptr ) )
          break;
      }
    return i;
  }


  template <typename T>
  inline
  bool DefaultAsciiItemReader<T>::readOne( DataSource & ds, T* pitem ) const
  {
    return AsciiDataSourceTraits<T>::read( ds, *pitem );
  }


}


#endif
