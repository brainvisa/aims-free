#ifndef AIMS_IO_JPEGFINDERFORMAT_H
#define AIMS_IO_JPEGFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderJpegFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}



#endif

