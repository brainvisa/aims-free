#ifndef AIMS_IO_GIFTIFINDERFORMAT_H
#define AIMS_IO_GIFTIFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderGiftiFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}



#endif

