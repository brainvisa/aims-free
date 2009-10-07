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


