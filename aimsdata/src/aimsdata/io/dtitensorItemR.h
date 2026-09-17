/*
 *  DtiTensor item reader class
 */
#ifndef AIMS_IO_DTITENSORITEMR_H
#define AIMS_IO_DTITENSORITEMR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/math/dtitensor.h>


namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemReader<DtiTensor> 
    : public ItemReader<DtiTensor>
  {
  public:
    DefaultItemReader() : ItemReader<DtiTensor>() {}
    virtual ~DefaultItemReader() {}

    virtual void read( std::istream & is, DtiTensor& item ) const;
    virtual void read( std::istream & is, DtiTensor *pitem, size_t n ) const;

    virtual ItemReader<DtiTensor>* 
      reader( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemReader<DtiTensor> 
    : public DefaultItemReader<DtiTensor>
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<DtiTensor>() {}
    virtual ~DefaultAsciiItemReader() {}

    virtual void read( std::istream & is, DtiTensor& item ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemReader<DtiTensor> 
    : public DefaultItemReader<DtiTensor>
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<DtiTensor>() {}
    virtual ~DefaultBSwapItemReader() {}

    virtual void read( std::istream & is, DtiTensor& item ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}	// namespace aims


#endif
