#ifndef AIMS_IO_ITEMR_STL_H
#define AIMS_IO_ITEMR_STL_H

#include <aims/io/defaultItemR.h>
#include <vector>

namespace aims
{

  ///	Partial specialization for vectors
  template <class T>
  class DefaultItemReader<std::vector<T> > 
    : public ItemReader<std::vector<T> >
  {
  public:
    DefaultItemReader() : ItemReader<std::vector<T> >() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & os, std::vector<T> & item ) const 
    { read( os, &item, 1 ); }
    virtual void read( std::istream & os, std::vector<T> * pitem, 
			size_t n ) const;
    virtual ItemReader<std::vector<T> >* reader( const std::string & openmode 
                                                 = "binar", 
                                                 bool bswap = false ) const;
  };


  ///	Partial specialization for vectors
  template <class T>
  class DefaultBSwapItemReader<std::vector<T> > 
    : public DefaultItemReader<std::vector<T> >
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<std::vector<T> >() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & os, std::vector<T> & item ) const 
    { read( os, &item, 1 ); }
    virtual void read( std::istream & os, std::vector<T> * pitem, 
                       size_t n ) const;
  };


  ///	Partial specialization for vectors
  template <class T>
  class DefaultAsciiItemReader<std::vector<T> > 
    : public DefaultItemReader<std::vector<T> >
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<std::vector<T> >() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & os, std::vector<T> & item ) const 
    { read( os, &item, 1 ); }
    virtual void read( std::istream & os, std::vector<T> * pitem, 
                       size_t n ) const;
  };


  // definitions

  template<class T> ItemReader<std::vector<T> >* 
  DefaultItemReader<std::vector<T> >::reader( const std::string & openmode, 
                                              bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemReader<std::vector<T> > );
    if( bswap )
      return( new DefaultBSwapItemReader<std::vector<T> > );
    return( new DefaultItemReader<std::vector<T> > );
  }


  template <class T> inline void
  DefaultItemReader<std::vector<T> >::read( std::istream & os, 
                                            std::vector<T>* pitem, 
                                            size_t n ) const
  {
    DefaultItemReader<T>	ir;
    unsigned			p;
    std::vector<T>		*ptr = pitem;
    DefaultItemReader<uint32_t>	ir2;
    uint32_t			nitem;

    for( p=0; p<n; ++p, ++ptr )
      {
        ir2.read( os, nitem );
        *ptr = std::vector<T>( nitem );
        ir.read( os, &(*ptr)[0], nitem );
      }
  }


  template <class T> inline void
  DefaultBSwapItemReader<std::vector<T> >::read( std::istream & os, 
                                                 std::vector<T>
                                                 * pitem, 
                                                 size_t n ) const
  {
    DefaultBSwapItemReader<T> 	ir;
    unsigned			p;
    std::vector<T>		*ptr = pitem;
    DefaultBSwapItemReader<uint32_t>	ir2;
    uint32_t			nitem;

    for( p=0; p<n; ++p, ++ptr )
      {
        ir2.read( os, nitem );
        *ptr = std::vector<T>( nitem );
        ir.read( os, &(*ptr)[0], nitem );
      }
  }


  template <class T> inline void
  DefaultAsciiItemReader<std::vector<T> >::read( std::istream & os, 
                                                 std::vector<T> * pitem, 
                                                 size_t n ) const
  {
    DefaultAsciiItemReader<T> 	ir;
    unsigned			p;
    std::vector<T>		*ptr = pitem;
    DefaultAsciiItemReader<uint32_t>	ir2;
    uint32_t			nitem;

    for( p=0; p<n; ++p, ++ptr )
      {
        ir2.read( os, nitem );
        *ptr = std::vector<T>( nitem );
        ir.read( os, &(*ptr)[0], nitem );
      }
  }

}

#endif
