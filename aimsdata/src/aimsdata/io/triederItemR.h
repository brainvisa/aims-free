/*
 *  Trieder item reader class
 */
#ifndef AIMS_IO_TRIEDERITEMR_H
#define AIMS_IO_TRIEDERITEMR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/math/trieder.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemReader<Trieder> : public ItemReader<Trieder>
  {
  public:
    DefaultItemReader() : ItemReader<Trieder>() { }
    virtual ~DefaultItemReader() { }

    virtual void read( std::istream & is, Trieder & pitem ) const;
    virtual void read( std::istream & is, Trieder * pitem, size_t n ) const;
    virtual ItemReader<Trieder>* 
      reader( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemReader<Trieder> 
    : public DefaultItemReader<Trieder>
  {
  public:
    DefaultAsciiItemReader() : DefaultItemReader<Trieder>() { }
    virtual ~DefaultAsciiItemReader() { }

    virtual void read( std::istream & is, Trieder & pitem ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemReader<Trieder> 
    : public DefaultItemReader<Trieder>
  {
  public:
    DefaultBSwapItemReader() : DefaultItemReader<Trieder>() { }
    virtual ~DefaultBSwapItemReader() { }

    virtual void read( std::istream & is, Trieder & pitem ) const;
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

}


#endif
