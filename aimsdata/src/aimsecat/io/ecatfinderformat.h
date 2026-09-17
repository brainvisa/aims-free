#ifndef AIMS_IO_ECATFINDERFORMAT_H
#define AIMS_IO_ECATFINDERFORMAT_H

#include <aims/io/finderFormats.h>

namespace aims
{

  class FinderEcatFormat : public FinderFormat
  {
  public:
    virtual bool check( const std::string & filename, Finder & f ) const;
  };

}



#endif

