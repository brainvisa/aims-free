/*
 *  DtiTensor item writer class
 */
#ifndef AIMS_IO_DTITENSORITEMW_H
#define AIMS_IO_DTITENSORITEMW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/math/dtitensor.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemWriter<DtiTensor> 
    : public ItemWriter<DtiTensor>
  {
  public:
    DefaultItemWriter() : ItemWriter<DtiTensor>() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & is, const DtiTensor & item ) const;
    virtual void write( std::ostream & is, const DtiTensor * pitem, 
			size_t n ) const;
    virtual ItemWriter<DtiTensor>* 
      writer( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemWriter<DtiTensor> 
    : public DefaultItemWriter<DtiTensor>
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<DtiTensor>() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & is, const DtiTensor & item ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemWriter<DtiTensor> 
    : public DefaultItemWriter<DtiTensor>
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<DtiTensor>() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & is, const DtiTensor & item ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}	// namespace aims


#endif
