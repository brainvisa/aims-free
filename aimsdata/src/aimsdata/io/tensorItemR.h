/*
 *  Tensor item reader class
 */
#ifndef AIMS_IO_TENSORITEMR_H
#define AIMS_IO_TENSORITEMR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/math/tensor.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemReader<Tensor> : public ItemReader<Tensor>
  {
  public:
    DefaultItemReader() : ItemReader<Tensor>() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, Tensor & pitem ) const;
    virtual void read( std::istream & is, Tensor * pitem, size_t n ) const;
    virtual ItemReader<Tensor>* 
      reader( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemReader<Tensor> 
    : public DefaultItemReader<Tensor>
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<Tensor>() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & is, Tensor & pitem ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemReader<Tensor> 
    : public DefaultItemReader<Tensor>
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<Tensor>() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, Tensor & pitem ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}


#endif
