#ifndef AIMS_APC_READER_H
#define AIMS_APC_READER_H

#include <cartobase/object/object.h>

namespace aims
{

  /// Reads .APC (anterior/posterior commissures) files
  class APCReader
  {
  public:
    APCReader( const std::string & filename );
    ~APCReader();
    /// returns a dictionary, may throw an exception when it fails
    carto::Object read() const;
    std::string filename() const;
    void setFilename( const std::string & );

  private:
    std::string	_filename;
  };

}

#endif

