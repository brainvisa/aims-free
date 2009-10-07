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

#ifndef CARTOBASE_IO_ITEMWRITER_H
#define CARTOBASE_IO_ITEMWRITER_H

#include <cartobase/datasource/datasource.h>
#include <cartobase/wip/io/asciidatasourcetraits.h>

namespace carto
{

  /**	Low-level "small item" writer, used by higher-level file readers. 
	\sa ItemReader
  */
  template<typename T>
  class ItemWriter
  {
  public:
    virtual ~ItemWriter() {}

    /// Factory function
    virtual ItemWriter<T>* writer( bool binary = true, 
				   bool bswap = false ) const = 0;

    bool write( DataSource & ds, const T & item ) const 
    { return write( ds, &item, 1 ) == 1; }
    virtual long write( DataSource & ds, const T* pitem, 
			size_t n ) const = 0;
  };


  /**	Default low-levels writers.
	They are a default implementation of the writers for most common types,
	and can be specialized for other types if needed.
	We have aleady specialized AimsVector writers
	\sa DefaultItemReader 
  */
  template <typename T>
  class DefaultItemWriter : public ItemWriter<T>
  {
  public:
    DefaultItemWriter() : ItemWriter<T>() { }
    virtual ~DefaultItemWriter() { }

    virtual long write( DataSource & ds, const T* pitem, size_t n ) const;
    virtual ItemWriter<T>* writer( bool binary = true, 
				   bool bswap = false ) const;
  };


  template <typename T>
  class DefaultAsciiItemWriter : public DefaultItemWriter<T>
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<T>() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual long write( DataSource & ds, const T* pitem, size_t n ) const;
  };


  template <typename T>
  class DefaultBSwapItemWriter : public DefaultItemWriter<T>
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<T>() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual long write( DataSource & ds, const T* pitem, size_t n ) const;
  };


  template<typename T> ItemWriter<T>* 
  DefaultItemWriter<T>::writer( bool binary, 
				bool bswap ) const
  {
    if( !binary )
      return( new DefaultAsciiItemWriter<T> );
    if( bswap )
      return( new DefaultBSwapItemWriter<T> );
    return( new DefaultItemWriter<T> );
  }


  template <typename T> inline long
  DefaultItemWriter<T>::write( DataSource & ds, const T* pitem, 
			       size_t n ) const
  {
    long len = ds.writeBlock( (const char *) pitem, sizeof(T) * n );
    return len / sizeof( T );
  }


  template <typename T> inline
  long DefaultAsciiItemWriter<T>::write( DataSource & ds, const T* pitem, 
					 size_t n ) const
  {
    const T* ptr = pitem;
    size_t	i;
    for( i=0; i<n && ds.isOpen(); ++i, ++ptr )
      ds << *ptr << " ";
    return (long) i;
  }


  template <typename T> inline
  long DefaultBSwapItemWriter<T>::write( DataSource & ds, const T* pitem, 
					 size_t n ) const
  {
    std::vector<uint8_t>	pd( n * sizeof( T ) );
    const uint8_t	*ps = (uint8_t *) pitem;
    for( size_t k=0; k<n*sizeof(T); k+=sizeof(T) )
      for( size_t b=0; b<sizeof(T)/2; ++b )
	pd[k+b] = ps[k+sizeof(T)-1-b];
    return ds.writeBlock( (const char *) &pd[0], sizeof(T) * n ) / sizeof(T);
  }


  //	specializations

  template <> inline long 
  DefaultItemWriter<Void>::write( DataSource &, const Void*, size_t n ) const
  {
    return (long) n;
  }


  template <> inline long 
  DefaultAsciiItemWriter<Void>::write( DataSource &, const Void*, 
				       size_t n ) const
  {
    return (long) n;
  }


  template <> inline long 
  DefaultBSwapItemWriter<Void>::write( DataSource &, const Void*, 
				       size_t n ) const
  {
    return (long) n;
  }

}


#endif

