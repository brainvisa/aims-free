/*
 *  Finder class
 */
#ifndef AIMS_IO_TRIHEADER_H
#define AIMS_IO_TRIHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>


namespace aims
{

  class AIMSDATA_API TriHeader : public PythonHeader
  {
  public:
    TriHeader( const std::string & filename );
    virtual ~TriHeader();

    /**	Reads the header, and if \c offset is not null, sets the file offset 
	to the data field */
    virtual bool read( size_t *offset = 0 );
    virtual std::string openMode() const;
    virtual bool byteSwapping() const;
    std::string filename() const;
    virtual std::string extension() const { return( ".tri" ); }
    virtual std::set<std::string> extensions() const;

  private:
    std::string	_filename;
  };

}


#endif

