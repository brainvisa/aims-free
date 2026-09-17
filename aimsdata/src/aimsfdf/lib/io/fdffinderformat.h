#ifndef AIMS_IO_FDFFINDERFORMAT_H
#define AIMS_IO_FDFFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderFdfFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}



#endif

