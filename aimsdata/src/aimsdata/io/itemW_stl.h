#ifndef AIMS_IO_ITEMW_STL_H
#define AIMS_IO_ITEMW_STL_H

#include <aims/io/defaultItemW.h>
#include <vector>

namespace aims
{

  ///	Partial specialization for vectors
  template <class T>
  class DefaultItemWriter<std::vector<T> > 
    : public ItemWriter<std::vector<T> >
  {
  public:
    DefaultItemWriter() : ItemWriter<std::vector<T> >() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & os, 
			const std::vector<T> & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const std::vector<T> * pitem, 
			size_t n ) const;
    virtual ItemWriter<std::vector<T> >* writer( const std::string & openmode 
                                                 = "binar", 
                                                 bool bswap = false ) const;
  };


  ///	Partial specialization for vectors
  template <class T>
  class DefaultBSwapItemWriter<std::vector<T> > 
    : public DefaultItemWriter<std::vector<T> >
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<std::vector<T> >() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & os, 
			const std::vector<T> & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const std::vector<T> * pitem, 
			size_t n ) const;
  };


  ///	Partial specialization for vectors
  template <class T>
  class DefaultAsciiItemWriter<std::vector<T> > 
    : public DefaultItemWriter<std::vector<T> >
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<std::vector<T> >() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & os, 
			const std::vector<T> & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const std::vector<T> * pitem, 
			size_t n ) const;
  };


  // definitions

  template<class T> ItemWriter<std::vector<T> >* 
  DefaultItemWriter<std::vector<T> >::writer( const std::string & openmode, 
                                                 bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemWriter<std::vector<T> > );
    if( bswap )
      return( new DefaultBSwapItemWriter<std::vector<T> > );
    return( new DefaultItemWriter<std::vector<T> > );
  }


  template <class T> inline void
  DefaultItemWriter<std::vector<T> >::write( std::ostream & os, 
                                             const std::vector<T>* pitem, 
                                             size_t n ) const
  {
    DefaultItemWriter<T>	ir;
    unsigned			p;
    const std::vector<T>	*ptr = pitem;
    DefaultItemWriter<uint32_t>	ir2;

    for( p=0; p<n; ++p, ++ptr )
      {
        ir2.write( os, ptr->size() );
        ir.write( os, &(*ptr)[0], ptr->size() );
      }
  }


  template <class T> inline void
  DefaultBSwapItemWriter<std::vector<T> >::write( std::ostream & os, 
                                                  const std::vector<T>
                                                  * pitem, 
                                                  size_t n ) const
  {
    DefaultBSwapItemWriter<T> 	ir;
    unsigned			p;
    const std::vector<T>	*ptr = pitem;
    DefaultBSwapItemWriter<uint32_t>	ir2;

    for( p=0; p<n; ++p, ++ptr )
      {
        ir2.write( os, ptr->size() );
        ir.write( os, &(*ptr)[0], ptr->size() );
      }
  }


  template <class T> inline void
  DefaultAsciiItemWriter<std::vector<T> >::write( std::ostream & os, 
                                                  const std::vector<T>
                                                  * pitem, 
                                                  size_t n ) const
  {
    DefaultAsciiItemWriter<T> 	ir;
    unsigned			p;
    const std::vector<T>	*ptr = pitem;
    DefaultAsciiItemWriter<uint32_t>	ir2;

    for( p=0; p<n; ++p, ++ptr )
      {
        ir2.write( os, ptr->size() );
        ir.write( os, &(*ptr)[0], ptr->size() );
      }
  }

}

#endif
