/*
 *  Tensor item writer class
 */
#ifndef AIMS_IO_TENSORITEMW_H
#define AIMS_IO_TENSORITEMW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/math/tensor.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemWriter<Tensor> : public ItemWriter<Tensor>
  {
  public:
    DefaultItemWriter() : ItemWriter<Tensor>() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & is, const Tensor & item ) const;
    virtual void write( std::ostream & is, const Tensor * pitem, 
			size_t n ) const;
    virtual ItemWriter<Tensor>* 
      writer( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemWriter<Tensor> 
    : public DefaultItemWriter<Tensor>
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<Tensor>() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & is, const Tensor & item ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemWriter<Tensor> 
    : public DefaultItemWriter<Tensor>
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<Tensor>() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & is, const Tensor & item ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}	// namespace aims


#endif
