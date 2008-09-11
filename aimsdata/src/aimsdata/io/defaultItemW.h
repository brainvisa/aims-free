/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Default item writer class
 */
#ifndef AIMS_IO_DEFAULTITEMW_H
#define AIMS_IO_DEFAULTITEMW_H


#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>
#include <aims/vector/vector.h>
#include <aims/rgb/rgb.h>
#include <aims/io/itemW.h>


namespace aims
{

  /**	Default low-levels writers.
	They are a default implementation of the writers for most common types,
	and can be specialized for other types if needed.
	We have aleady specialized AimsVector writers
	\sa DefaultItemReader 
  */
  template <class T>
  class AIMSDATA_API DefaultItemWriter : public ItemWriter<T>
  {
  public:
    DefaultItemWriter() : ItemWriter<T>() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & os, const T & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const T* pitem, size_t n ) const;
    virtual ItemWriter<T>* writer( const std::string & openmode = "binar", 
				   bool bswap = false ) const;
  };


  template <class T>
  class AIMSDATA_API DefaultAsciiItemWriter : public DefaultItemWriter<T>
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<T>() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & os, const T& item ) const;
    virtual void write( std::ostream & os, const T* pitem, size_t n ) const;
  };


  template <class T>
  class AIMSDATA_API DefaultBSwapItemWriter : public DefaultItemWriter<T>
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<T>() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & os, const T& item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const T* pitem, size_t n ) const;
  };


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  ///	Partial specialization for vectors
  template <class T, int D>
  class AIMSDATA_API DefaultItemWriter<AimsVector<T,D> > 
    : public ItemWriter<AimsVector<T,D> >
  {
  public:
    DefaultItemWriter() : ItemWriter<AimsVector<T,D> >() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & os, 
			const AimsVector<T,D> & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const AimsVector<T,D> * pitem, 
			size_t n ) const;
    virtual ItemWriter<AimsVector<T,D> >* writer( const std::string & openmode 
						  = "binar", 
						  bool bswap = false ) const;
  };


  ///	Partial specialization for vectors
  template <class T, int D>
  class AIMSDATA_API DefaultBSwapItemWriter<AimsVector<T,D> > 
    : public DefaultItemWriter<AimsVector<T,D> >
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<AimsVector<T,D> >() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & os, 
			const AimsVector<T,D> & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const AimsVector<T,D> * pitem, 
			size_t n ) const;
  };

#endif


  template<class T> ItemWriter<T>* 
  DefaultItemWriter<T>::writer( const std::string & openmode, 
				bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemWriter<T> );
    if( bswap )
      return( new DefaultBSwapItemWriter<T> );
    return( new DefaultItemWriter<T> );
  }


  template <class T> inline void
  DefaultItemWriter<T>::write( std::ostream & os, const T* pitem, 
			       size_t n ) const
  {
    os.write( (const char *) pitem, sizeof(T) * n );
  }


  template <class T> inline
  void DefaultAsciiItemWriter<T>::write( std::ostream & os, 
					 const T& item ) const
  {
    os << item << " ";
  }


  template <class T> inline
  void DefaultAsciiItemWriter<T>::write( std::ostream & os, const T* pitem, 
					 size_t n ) const
  {
    const T* ptr = pitem;
    for( size_t i=0; i<n; ++i, ++ptr )
      os << *ptr << " ";
  }


  template <class T> inline
  void DefaultBSwapItemWriter<T>::write( std::ostream & os, const T* pitem, 
					 size_t n ) const
  {
    byte	*pd = new byte[ n * sizeof( T ) ];
    const byte	*ps = (byte *) pitem;
    for( size_t k=0; k<n*sizeof(T); k+=sizeof(T) )
      for( size_t b=0; b<sizeof(T)/2; ++b )
	pd[k+b] = ps[k+sizeof(T)-1-b];
    os.write( (const char *) pd, sizeof(T) * n );
    delete[] pd;
  }


  //	partial specializations

  template<class T, int D> ItemWriter<AimsVector<T, D> >* 
  DefaultItemWriter<AimsVector<T, D> >::writer( const std::string & openmode, 
						bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemWriter<AimsVector<T, D> > );
    if( bswap )
      return( new DefaultBSwapItemWriter<AimsVector<T, D> > );
    return( new DefaultItemWriter<AimsVector<T, D> > );
  }


  template <class T, int D> inline void
  DefaultItemWriter<AimsVector<T, D> >::write( std::ostream & os, 
					       const AimsVector<T, D>* pitem, 
					       size_t n ) const
  {
    DefaultItemWriter<T>	ir;
    const AimsVector<T,D>	*ptr = pitem;

    for( size_t p=0; p<n; ++p, ++ptr )
      ir.write( os, &(*ptr)[0], D );
  }


  template <class T, int D> inline void
  DefaultBSwapItemWriter<AimsVector<T, D> >::write( std::ostream & os, 
						    const AimsVector<T, D>
						    * pitem, 
						    size_t n ) const
  {
    DefaultBSwapItemWriter<T> 	ir;
    const AimsVector<T,D>	*ptr = pitem;

    for( size_t p=0; p<n; ++p, ++ptr )
      ir.write( os, &(*ptr)[0], D );
  }


  //	specializations

  template <> inline void 
  DefaultItemWriter<Void>::write( std::ostream &, const Void*, size_t ) const
  {
  }


  template <> inline void
  DefaultAsciiItemWriter<Void>::write( std::ostream & , const Void*, 
				       size_t ) const
  {
  }


  template <> inline void
  DefaultBSwapItemWriter<Void>::write( std::ostream &, const Void*, 
				       size_t ) const
  {
  }


  template <> inline void 
  DefaultBSwapItemWriter<int8_t>::write( std::ostream & os, 
                                         const int8_t* pitem, size_t n ) const
  {
    os.write( (const char *) pitem, n );
  }


  template <> inline void
  DefaultBSwapItemWriter<uint8_t>::write( std::ostream & os, 
                                          const uint8_t* pitem, 
                                          size_t n ) const
  {
    os.write( (const char *) pitem, n );
  }


  template <> inline void 
  DefaultBSwapItemWriter<bool>::write( std::ostream & os, const bool* pitem, 
				       size_t n ) const
  {
    os.write( (const char *) pitem, n );
  }


  template <> inline void
  DefaultBSwapItemWriter<cfloat>::write( std::ostream & os, 
					 const cfloat* pitem, size_t n ) const
  {
    byte	*pd = new byte[ n * sizeof( cfloat ) ];
    const byte	*ps = (byte*) pitem;
    for( size_t k=0; k<n*sizeof(cfloat); k+=sizeof(float) )
      for( size_t b=0; b<sizeof(float)/2; ++b )
	pd[k+b] = ps[k+sizeof(float)-1-b];
    os.write( (const char *) pd, n );
    delete[] pd;
  }


  template <> inline void
  DefaultBSwapItemWriter<cdouble>::write( std::ostream & os, 
					  const cdouble* pitem, 
					  size_t n ) const
  {
    byte	*pd = new byte[ n * sizeof( cdouble ) ];
    const byte	*ps = (byte*) pitem;
    for( size_t k=0; k<n*sizeof(cdouble); k+=sizeof(float) )
      for( size_t b=0; b<sizeof(double)/2; ++b )
	pd[k+b] = ps[k+sizeof(double)-1-b];
    os.write( (const char *) pd, n );
    delete[] pd;
  }


  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  template <> inline void
  DefaultBSwapItemWriter<char>::write( std::ostream & os, 
                                       const char* pitem, 
                                       size_t n ) const
  {
    os.write( (const char *) pitem, n );
  }
#endif


}


#endif
