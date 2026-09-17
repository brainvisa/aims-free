/*
 *  Bucket item reader class
 */
#ifndef AIMS_IO_BCKITEMW_H
#define AIMS_IO_BCKITEMW_H


#include <aims/io/defaultItemW.h>
#include <aims/bucket/item.h>


namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template<class T> class DefaultItemWriter<AimsBucketItem<T> > 
    : public ItemWriter<AimsBucketItem<T> >
  {
  public:
    DefaultItemWriter() : ItemWriter<AimsBucketItem<T> >() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & is, 
			const AimsBucketItem<T> & item ) const;
    virtual void write( std::ostream & is, const AimsBucketItem<T> * pitem, 
			size_t n ) const;
    virtual ItemWriter<AimsBucketItem<T> >* 
      writer( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template<class T> class DefaultAsciiItemWriter<AimsBucketItem<T> > 
    : public DefaultItemWriter<AimsBucketItem<T> >
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<AimsBucketItem<T> >() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & is, 
			const AimsBucketItem<T> & item ) const;
  };


  template<class T> class DefaultBSwapItemWriter<AimsBucketItem<T> > 
    : public DefaultItemWriter<AimsBucketItem<T> >
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<AimsBucketItem<T> >() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & is, 
			const AimsBucketItem<T> & item ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


  //	definitions

  template<class T> ItemWriter<AimsBucketItem<T> >* 
  DefaultItemWriter<AimsBucketItem<T> >::writer( const std::string & openmode, 
						 bool bswap ) const
  {
    if( openmode == "ascii" )
      return( new DefaultAsciiItemWriter<AimsBucketItem<T> > );
    if( bswap )
      return( new DefaultBSwapItemWriter<AimsBucketItem<T> > );
    return( new DefaultItemWriter<AimsBucketItem<T> > );
  }


  template<class T>
  void DefaultItemWriter<AimsBucketItem<T> >::write
  ( std::ostream & is, const AimsBucketItem<T>* pitem, size_t n ) const
  {
    const AimsBucketItem<T>	*ptr = pitem;
    for( size_t i=0; i<n; ++i )
      write( is, *ptr++ );
  }


  template<class T> void 
  DefaultItemWriter<AimsBucketItem<T> >::write
  (
   std::ostream & os, const AimsBucketItem<T> & item ) const
    {
      DefaultItemWriter<Point3d>		itemW1;
      DefaultItemWriter<T>		itemW2;

      itemW1.write( os, item.location() );
      itemW2.write( os, item.value() );
    }


  template<class T> void DefaultAsciiItemWriter<AimsBucketItem<T> >::write
  ( std::ostream & os, const AimsBucketItem<T> & item ) const
  {
    DefaultAsciiItemWriter<Point3d>	itemW1;
    DefaultAsciiItemWriter<T>		itemW2;

    itemW1.write( os, item.location() );
    itemW2.write( os, item.value() );
  }


  template<class T> void DefaultBSwapItemWriter<AimsBucketItem<T> >::write
  ( std::ostream & os, const AimsBucketItem<T> & item ) const
  {
    DefaultBSwapItemWriter<Point3d>	itemW1;
    DefaultBSwapItemWriter<T>		itemW2;

    itemW1.write( os, item.location() );
    itemW2.write( os, item.value() );
  }

}


#endif


