#ifndef AIMS_IO_PLYHEADER_H
#define AIMS_IO_PLYHEADER_H

#include <aims/data/pheader.h>

namespace aims
{

  class PlyHeader : public PythonHeader
  {
  public:
    PlyHeader( const std::string & filename );
    virtual ~PlyHeader();

    /**	Reads the header, and if \c offset is not null, sets the file offset 
	to the data field */
    virtual bool read( uint32_t *offset = 0 );
    std::string filename() const;
    virtual std::string extension() const { return( ".ply" ); }
    virtual std::set<std::string> extensions() const;

  private:
    std::string	_filename;
  };

}

#endif

