#ifndef AIMS_IO_TIFFFINDERFORMAT_H
#define AIMS_IO_TIFFFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderTiffFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}



#endif

