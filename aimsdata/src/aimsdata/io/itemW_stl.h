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
