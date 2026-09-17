/*
 *  Trider item writer class
 */
#ifndef AIMS_IO_TRIEDERITEMW_H
#define AIMS_IO_TRIEDERITEMW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/math/trieder.h>

namespace aims
{

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <>
  class AIMSDATA_API DefaultItemWriter<Trieder> : public ItemWriter<Trieder>
  {
  public:
    DefaultItemWriter() : ItemWriter<Trieder>() { }
    virtual ~DefaultItemWriter() { }

    virtual void write( std::ostream & is, const Trieder & item ) const;
    virtual void write( std::ostream & is, const Trieder * pitem, 
			size_t n ) const;
    virtual ItemWriter<Trieder>* 
      writer( const std::string & openmode = "binar", 
	      bool bswap = false ) const;
  };


  template <>
  class AIMSDATA_API DefaultAsciiItemWriter<Trieder> 
    : public DefaultItemWriter<Trieder>
  {
  public:
    DefaultAsciiItemWriter() : DefaultItemWriter<Trieder>() { }
    virtual ~DefaultAsciiItemWriter() { }

    virtual void write( std::ostream & is, const Trieder & item ) const;
  };


  template <>
  class AIMSDATA_API DefaultBSwapItemWriter<Trieder> 
    : public DefaultItemWriter<Trieder>
  {
  public:
    DefaultBSwapItemWriter() : DefaultItemWriter<Trieder>() { }
    virtual ~DefaultBSwapItemWriter() { }

    virtual void write( std::ostream & is, const Trieder & item ) const;
  };

#endif

}	// namespace aims


#endif
