/*
 *  Header class
 */
#ifndef AIMS_DATA_HEADER_H
#define AIMS_DATA_HEADER_H

#define AIMSDATA_NEW_AIMSDATA

#include <aims/config/aimsdata_config.h>
#include <set>
#include <string>

namespace aims
{

  class AIMSDATA_API Header
  {
  public:
    virtual const char* id() const;
    virtual ~Header()=0;
    virtual Header* cloneHeader( bool keepUuid = false ) const = 0;

    ///	standard file format extension of specialized headers
    virtual std::string extension() const;
    /// possible filename extensions for specialized formats
    virtual std::set<std::string> extensions() const;
    /// extension of given filename (including '.')
    virtual std::string extension( const std::string & filename ) const;
    virtual std::string removeExtension( const std::string & ) const;

    /// read meta-info header
    virtual bool readMinf( const std::string & filename );
    /// write meta-info header, non-const version (may change some attributes)
    virtual bool writeMinf( const std::string & filename );
    /// write meta-info header
    virtual bool writeMinf( const std::string & filename ) const;
  };

}

#endif
