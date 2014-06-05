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

/*
 *  Data item reader class
 */
#ifndef AIMS_IO_DEFAULTITEMR_H
#define AIMS_IO_DEFAULTITEMR_H

#include <aims/def/general.h>
#include <aims/vector/vector.h>
#include <aims/io/itemR.h>
#include <soma-io/utilities/asciidatasourcetraits.h>
#include <algorithm>


namespace aims
{

  /**	Default low-levels readers.
	They are a default implementation of the readers for most common types,
	and can be specialized for other types if needed.
	We have aleady specialized AimsVector readers */
  template <class T>
  class DefaultItemReader : public ItemReader<T>
  {
  public:
    DefaultItemReader() : ItemReader<T>() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, T& item ) const
    { read( is, &item, 1 ); }
    virtual void read( std::istream & is, T* pitem, size_t n ) const;
    virtual ItemReader<T>* reader( const std::string & openmode = "binar", 
				   bool bswap = false ) const;
    inline void read( const std::string &istring, T &item ) const
      { ItemReader<T>::read( istring, item ); }
    inline void read( const std::string &istring, T *pitem, size_t n ) const
      { ItemReader<T>::read( istring, pitem, n ); }
  };


  template <class T>
  class DefaultAsciiItemReader : public DefaultItemReader<T>
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<T>() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & is, T& item ) const;
    virtual void read( std::istream & is, T* pitem, size_t n ) const;
    inline void read( const std::string &istring, T &item ) const
      { ItemReader<T>::read( istring, item ); }
    inline void read( const std::string &istring, T *pitem, size_t n ) const
      { ItemReader<T>::read( istring, pitem, n ); }
  };


  template <class T>
  class DefaultBSwapItemReader : public DefaultItemReader<T>
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<T>() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, T& item ) const
    { read( is, &item, 1 ); }
    virtual void read( std::istream & is, T* pitem, size_t n ) const;
    inline void read( const std::string &istring, T &item ) const
      { ItemReader<T>::read( istring, item ); }
    inline void read( const std::string &istring, T *pitem, size_t n ) const
      { ItemReader<T>::read( istring, pitem, n ); }
  };


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  ///	Partial specialization for vectors
  template <class T, int D>
  class DefaultItemReader<AimsVector<T,D> > 
    : public ItemReader<AimsVector<T,D> >
  {
  public:
    DefaultItemReader() : ItemReader<AimsVector<T,D> >() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, AimsVector<T,D> & item ) const 
    { read( is, &item, 1 ); }
    virtual void read( std::istream & is, AimsVector<T,D> * pitem, 
		       size_t n ) const;
    virtual ItemReader<AimsVector<T,D> >* reader( const std::string & openmode 
						  = "binar", 
						  bool bswap = false ) const;
    inline void read( const std::string &istring, T &item ) const
      { ItemReader<T>::read( istring, item ); }
    inline void read( const std::string &istring, T *pitem, size_t n ) const
      { ItemReader<T>::read( istring, pitem, n ); }
  };


  ///	Partial specialization for vectors
  template <class T, int D>
  class DefaultBSwapItemReader<AimsVector<T,D> > 
    : public DefaultItemReader<AimsVector<T,D> >
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<AimsVector<T,D> >() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, AimsVector<T,D> & item ) const 
    { read( is, &item, 1 ); }
    virtual void read( std::istream & is, AimsVector<T,D> * pitem, 
		       size_t n ) const;
    inline void read( const std::string &istring, T &item ) const
      { ItemReader<T>::read( istring, item ); }
    inline void read( const std::string &istring, T *pitem, size_t n ) const
      { ItemReader<T>::read( istring, pitem, n ); }
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


  template<class T> ItemReader<T>* 
  DefaultItemReader<T>::reader( const std::string & openmode, 
				bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemReader<T> );
    if( bswap )
      return( new DefaultBSwapItemReader<T> );
    return( new DefaultItemReader<T> );
  }


  template <class T> inline void 
  DefaultItemReader<T>::read( std::istream & is, T* pitem, size_t n ) const
  {
    is.read( (char *) pitem, sizeof(T) * n );
  }


  template <class T> inline
  void DefaultAsciiItemReader<T>::read( std::istream & is, T& item ) const
  {
    carto::StreamUtil::skip( is, " \t\n\r,", true );
    is >> item;
  }


  template <class T> inline
  void DefaultAsciiItemReader<T>::read( std::istream & is, T* pitem, 
					size_t n ) const
  {
    T* ptr = pitem;
    for( size_t i=0; i<n; ++i, ++ptr )
      read( is, *ptr );
  }


  template <class T> inline
  void DefaultBSwapItemReader<T>::read( std::istream & is, T* pitem, 
					size_t n ) const
  {
    is.read( (char *) pitem, sizeof(T) * n );
    byte* ptr = (byte*) pitem;
    for( size_t k=0; k<n*sizeof(T); k+=sizeof(T) )
      for( size_t b=0; b<sizeof(T)/2; ++b )
	std::swap( ptr[k+b], ptr[k+sizeof(T)-1-b] );
  }


  //	partial specializations

  template<class T, int D> ItemReader<AimsVector<T, D> >* 
  DefaultItemReader<AimsVector<T, D> >::reader( const std::string & openmode, 
						bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemReader<AimsVector<T, D> > );
    if( bswap )
      return( new DefaultBSwapItemReader<AimsVector<T, D> > );
    return( new DefaultItemReader<AimsVector<T, D> > );
  }


  template <class T, int D> inline
  void DefaultItemReader<AimsVector<T, D> >::read( std::istream & is, 
						   AimsVector<T, D>* pitem, 
						   size_t n ) const
  {
    DefaultItemReader<T>	ir;
    AimsVector<T,D>		*ptr = pitem;

    for( size_t p=0; p<n; ++p, ++ptr )
      //for( i=0; i<D; ++i )
      ir.read( is, &(*ptr)[0], D );
  }


  template <class T, int D> inline void 
  DefaultBSwapItemReader<AimsVector<T, D> >::read( std::istream & is, 
						   AimsVector<T, D>* pitem, 
						   size_t n ) const
  {
    DefaultBSwapItemReader<T> 	ir;
    AimsVector<T,D>		*ptr = pitem;

    for( size_t p=0; p<n; ++p, ++ptr )
      //for( i=0; i<D; ++i )
      ir.read( is, &(*ptr)[0], D );
  }


  //	specializations

  template <> inline
  void DefaultItemReader<Void>::read( std::istream &, Void & ) const
  {
  }


  template <> inline
  void DefaultItemReader<Void>::read( std::istream &, Void*, size_t ) const
  {
  }


  template <> inline void 
  DefaultAsciiItemReader<Void>::read( std::istream & , Void & ) const
  {
  }


  template <> inline void 
  DefaultAsciiItemReader<Void>::read( std::istream & , Void*, size_t ) const
  {
  }


  template <> inline void 
  DefaultBSwapItemReader<Void>::read( std::istream &, Void & ) const
  {
  }


  template <> inline void 
  DefaultBSwapItemReader<Void>::read( std::istream &, Void*, size_t ) const
  {
  }


  template <> inline
  void DefaultBSwapItemReader<int8_t>::read( std::istream & is, int8_t* pitem, 
                                             size_t n ) const
  {
    is.read( (char *) pitem, n );
  }


  template <> inline
  void DefaultBSwapItemReader<uint8_t>::read( std::istream & is, 
                                              uint8_t* pitem, 
                                              size_t n ) const
  {
    is.read( (char *) pitem, n );
  }


  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  template <> inline
  void DefaultBSwapItemReader<char>::read( std::istream & is, 
                                           char* pitem, 
                                           size_t n ) const
  {
    is.read( (char *) pitem, n );
  }
#endif


  template <> inline
  void DefaultBSwapItemReader<bool>::read( std::istream & is, bool* pitem, 
					   size_t n ) const
  {
    is.read( (char *) pitem, n );
  }


  template <> inline
  void DefaultBSwapItemReader<cfloat>::read( std::istream & is, cfloat* pitem, 
					     size_t n ) const
  {
    is.read( (char *) pitem, n );
    byte *ptr = (byte*) pitem;
    for( size_t k=0; k<n*sizeof(cfloat); k+=sizeof(float) )
      for( size_t b=0; b<sizeof(float)/2; ++b )
	std::swap( ptr[k+b], ptr[k+sizeof(float)-1-b] );
  }


  template <> inline void 
  DefaultBSwapItemReader<cdouble>::read( std::istream & is, cdouble* pitem, 
					      size_t n ) const
  {
    is.read( (char *) pitem, n );
    byte* ptr = (byte*) pitem;
    for( size_t k=0; k<n*sizeof(cdouble); k+=sizeof(float) )
      for( size_t b=0; b<sizeof(double)/2; ++b )
	std::swap( ptr[k+b], ptr[k+sizeof(double)-1-b] );
  }


}


#endif

