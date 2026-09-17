/*
 *  Bucket item reader class
 */
#ifndef AIMS_IO_BCKITEMR_H
#define AIMS_IO_BCKITEMR_H


#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/bucket/item.h>


namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template<class T> 
  class AIMSDATA_API DefaultItemReader<AimsBucketItem<T> > 
    : public ItemReader<AimsBucketItem<T> >
  {
  public:
    DefaultItemReader() : ItemReader<AimsBucketItem<T> >() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, AimsBucketItem<T> * pitem, 
		       size_t n ) const;
    virtual ItemReader<AimsBucketItem<T> >* 
    reader( const std::string & openmode = "binar", bool bswap = false ) const;
  };


  template <class T>
  class AIMSDATA_API DefaultAsciiItemReader<AimsBucketItem<T> > 
    : public DefaultItemReader<AimsBucketItem<T> >
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<AimsBucketItem<T> >() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & is, AimsBucketItem<T> * pitem, 
		       size_t n ) const;
  };


  template <class T>
  class AIMSDATA_API DefaultBSwapItemReader<AimsBucketItem<T> > 
    : public DefaultItemReader<AimsBucketItem<T> >
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<AimsBucketItem<T> >() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, AimsBucketItem<T> * pitem, 
		       size_t n ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

  //	definitions

  template <class T> ItemReader<AimsBucketItem<T> >* 
  DefaultItemReader<AimsBucketItem<T> >::reader( const std::string & openmode, 
						 bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemReader<AimsBucketItem<T> > );
    if( bswap )
      return( new DefaultBSwapItemReader<AimsBucketItem<T> > );
    return( new DefaultItemReader<AimsBucketItem<T> > );
  }

  template <class T> inline
  void DefaultItemReader<AimsBucketItem<T> >::read( std::istream & is, 
						    AimsBucketItem<T>* pitem, 
						    size_t n ) const
  {
    DefaultItemReader<T>	ir;
    DefaultItemReader<Point3d>	pr;
    unsigned			p;
    AimsBucketItem<T>		*ptr = pitem;

    for( p=0; p<n; ++p, ++ptr )
      {
	pr.read( is, ptr->location() );
	ir.read( is, ptr->value() );
      }
  }


  template <class T> inline void 
  DefaultAsciiItemReader<AimsBucketItem<T> >::read( std::istream & is, 
						    AimsBucketItem<T>* pitem, 
						    size_t n ) const
  {
    DefaultAsciiItemReader<T>				ir;
    DefaultAsciiItemReader<AimsVector<short,3> >	pr;
    unsigned						p;
    AimsBucketItem<T>					*ptr = pitem;

    for( p=0; p<n; ++p, ++ptr )
      {
	pr.read( is, ptr->location() );
	ir.read( is, ptr->value() );
      }
  }


  template <class T> inline void 
  DefaultBSwapItemReader<AimsBucketItem<T> >::read( std::istream & is, 
						    AimsBucketItem<T>* pitem, 
						    size_t n ) const
  {
    DefaultBSwapItemReader<T>				ir;
    DefaultBSwapItemReader<AimsVector<short,3> >	pr;
    unsigned						p;
    AimsBucketItem<T>					*ptr = pitem;

    for( p=0; p<n; ++p, ++ptr )
      {
	pr.read( is, ptr->location() );
	ir.read( is, ptr->value() );
      }
  }


}


#endif

