#ifndef AIMS_IO_PLYFINDERFORMAT_H
#define AIMS_IO_PLYFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderPlyFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}

#endif

