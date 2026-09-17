/*
 *  Data item writer class
 */
#ifndef AIMS_IO_ITEMW_H
#define AIMS_IO_ITEMW_H

#include <string>
#include <iostream>

namespace aims
{

  /**	Low-level "small item" writer, used by higher-level file readers. 
	\sa ItemReader
  */
  template<class T>
  class ItemWriter
  {
  public:
    virtual ~ItemWriter() {}

    virtual void write( std::ostream & os, const T & item ) const 
    { write( os, &item, 1 ); }
    virtual void write( std::ostream & os, const T* pitem, 
			size_t n ) const = 0;
    virtual ItemWriter<T>* writer( const std::string & openmode = "binar", 
				   bool bswap = false ) const = 0;
  };

}


#endif

