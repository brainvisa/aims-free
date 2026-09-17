#ifndef AIMS_IO_VIDAFINDERFORMAT_H
#define AIMS_IO_VIDAFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderVidaFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}



#endif

