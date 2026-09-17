/*
 *  List item writer class
 */
#ifndef AIMS_IO_LISTITEMW_H
#define AIMS_IO_LISTITEMW_H

#include <aims/io/defaultItemW.h>
#include <list>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template<typename T>
  class DefaultItemWriter<std::list<T> > : public ItemWriter<std::list<T> >
  {
  public:
    DefaultItemWriter() : ItemWriter<std::list<T> >() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & is, const std::list<T> & item ) const;
    virtual void write( std::ostream & is, const std::list<T> * pitem, 
			size_t n ) const;
    virtual ItemWriter<std::list<T> >* 
      writer( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template<typename T>
  class DefaultAsciiItemWriter<std::list<T> > 
    : public DefaultItemWriter<std::list<T> >
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<std::list<T> >() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & is, const std::list<T> & item ) const;
  };


  template<typename T>
  class DefaultBSwapItemWriter<std::list<T> > 
    : public DefaultItemWriter<std::list<T> >
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<std::list<T> >() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & is, const std::list<T> & item ) const;
  };


  template<typename T> inline 
  ItemWriter<std::list<T> >* 
  DefaultItemWriter<std::list<T> >::writer( const std::string & openmode, 
					    bool bswap ) const
  {
    if ( openmode == "ascii" )
      return new DefaultAsciiItemWriter<std::list<T> >;
    if ( bswap )
      return new DefaultBSwapItemWriter<std::list<T> >;
    return new DefaultItemWriter<std::list<T> >;
  }


  template<typename T> inline 
  void DefaultItemWriter<std::list<T> >::write( std::ostream & is, 
						const std::list<T> * pitem, 
						size_t n ) const
  {
    const std::list<T>	*ptr = pitem;
    for( size_t i=0; i<n; ++i )
      write( is, *ptr++ );
  }


  template<typename T> inline void 
  DefaultItemWriter<std::list<T> >::write( std::ostream & os, 
					   const std::list<T> & item ) const
  {
    typename std::list<T>::const_iterator	it, et = item.end();
    DefaultItemWriter<uint32_t>		iw1;
    DefaultItemWriter<T>		iw;
    iw1.write( os, (uint32_t)item.size() );
    for ( it=item.begin(); it!=et; ++it )
      iw.write( os, *it );
  }


  template<typename T> inline void 
  DefaultAsciiItemWriter<std::list<T> >::write
  ( std::ostream & os, const std::list<T> & item ) const
  {
    typename std::list<T>::const_iterator	it, et = item.end();
    DefaultAsciiItemWriter<uint32_t>	iw1;
    DefaultAsciiItemWriter<T>		iw;
    os << '(';
    iw1.write( os, (uint32_t)item.size() );
    for ( it=item.begin(); it!=et; ++it )
      {
	os << ',';
	iw.write( os, *it );
      }
    os << ')';
  }


  template<typename T> inline void 
  DefaultBSwapItemWriter<std::list<T> >::write
  ( std::ostream & os, const std::list<T> & item ) const
  {
    typename std::list<T>::const_iterator	it, et = item.end();
    DefaultBSwapItemWriter<uint32_t>	iw1;
    DefaultBSwapItemWriter<T>		iw;
    iw1.write( os, (uint32_t)item.size() );
    for ( it=item.begin(); it!=et; ++it )
      iw.write( os, *it );
  }

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

} // namespace aims


#endif
