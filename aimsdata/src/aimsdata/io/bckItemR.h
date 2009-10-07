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

