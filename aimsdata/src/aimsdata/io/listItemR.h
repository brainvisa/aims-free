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
