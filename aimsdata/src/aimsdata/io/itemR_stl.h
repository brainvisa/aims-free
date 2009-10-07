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
