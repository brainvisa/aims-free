#ifndef AIMS_IO_IMASPARSEHEADER_H
#define AIMS_IO_IMASPARSEHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>


namespace aims
{

  class ImasHeader : public PythonHeader
  {
  public:
    ImasHeader( const std::string & filename );
    virtual ~ImasHeader();

    /** Reads the header, and if \c offset is not null, sets the file offset
        to the data field */
    virtual bool read( uint32_t *offset = 0 );
    virtual std::string openMode() const;
    virtual bool byteSwapping() const;
    std::string filename() const;
    virtual std::string extension() const { return( ".imas" ); }
    virtual std::set<std::string> extensions() const;

  private:
    std::string _filename;
  };

}


#endif

