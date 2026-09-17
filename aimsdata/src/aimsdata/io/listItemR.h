/*
 *  List item reader class
 */
#ifndef AIMS_IO_LISTITEMR_H
#define AIMS_IO_LISTITEMR_H

#include <aims/io/defaultItemR.h>
#include <list>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <typename T> class DefaultItemReader<std::list<T> > 
    : public ItemReader<std::list<T> >
  {
  public:
    DefaultItemReader() : ItemReader<std::list<T> >() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, std::list<T> & item ) const;
    virtual void read( std::istream & is, std::list<T> * pitem, 
		       size_t n ) const;
    virtual ItemReader<std::list<T> >* reader( const std::string & openmode 
					       = "binar", 
					       bool bswap = false ) const;
  };


  template <typename T> class DefaultAsciiItemReader<std::list<T> > 
    : public DefaultItemReader<std::list<T> >
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<std::list<T> >() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & is, std::list<T> & item ) const;
  };


  template <typename T> class DefaultBSwapItemReader<std::list<T> > 
    : public DefaultItemReader<std::list<T> >
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<std::list<T> >() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, std::list<T> & item ) const;
  };


  template<class T> ItemReader<std::list<T> >* 
  DefaultItemReader<std::list<T> >::reader( const std::string & openmode, 
					    bool bswap ) const
  {
    if( openmode == "ascii" )
      return new DefaultAsciiItemReader<std::list<T> >;
    if( bswap )
      return new DefaultBSwapItemReader<std::list<T> >;
    return new DefaultItemReader<std::list<T> >;
  }


  template <class T> inline void 
  DefaultItemReader<std::list<T> >::read( std::istream & is, 
					  std::list<T> * pitem, 
					  size_t n ) const
  {
    std::list<T>	*ptr = pitem;
    for( size_t i=0; i<n; ++i )
      read( is, *ptr++ );
  }


  template <class T> inline void 
  DefaultItemReader<std::list<T> >::read( std::istream & is, 
					  std::list<T> & item ) const
  {
    uint32_t			n;
    T				data;
    DefaultItemReader<uint32_t>	ir1;
    DefaultItemReader<T>	ir;
    ir1.read( is, n );
    for( uint i=0; i<n; ++i )
      {
	ir.read( is, data );
	item.push_back( data );
      }
  }


  template <class T> inline void 
  DefaultAsciiItemReader<std::list<T> >::read( std::istream & is, 
					       std::list<T> & item ) const
  {
    uint32_t				n;
    T					data;
    DefaultAsciiItemReader<uint32_t>	ir1;
    DefaultAsciiItemReader<T>		ir;
    char tmp;
    is >> tmp;
    // ASSERT( tmp == '(' );
    ir1.read( is, n );
    for( uint i=0; i<n; ++i )
      {
	is >> tmp;
	// ASSERT( tmp == ',' );
	ir.read( is, data );
	item.push_back( data );
      }
    is >> tmp;
    // ASSERT( tmp == ')' );
  }


  template <class T> inline void 
  DefaultBSwapItemReader<std::list<T> >::read( std::istream & is, 
					       std::list<T> & item ) const
  {
    uint32_t				n;
    T					data;
    DefaultBSwapItemReader<uint32_t>	ir1;
    DefaultBSwapItemReader<T>		ir;
    ir1.read( is, n );
    for( uint i=0; i<n; ++i )
      {
	ir.read( is, data );
	item.push_back( data );
      }
  }

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}	// namespace aims


#endif
