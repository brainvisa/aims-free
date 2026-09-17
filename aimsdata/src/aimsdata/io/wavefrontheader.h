/*
 *  Finder class
 */
#ifndef AIMS_IO_WAVEFRONTHEADER_H
#define AIMS_IO_WAVEFRONTHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>


namespace aims
{

  class WavefrontHeader : public PythonHeader
  {
  public:
    WavefrontHeader( const std::string & filename );
    virtual ~WavefrontHeader();

    /** Reads the header, and if \c offset is not null, sets the file offset
        to the data field */
    virtual bool read( size_t *offset = 0 );
    virtual std::string openMode() const;
    virtual bool byteSwapping() const;
    std::string filename() const;
    virtual std::string extension() const { return( ".obj" ); }
    virtual std::set<std::string> extensions() const;

  private:
    void readMesh( bool stopWhenIdentified, int poly_size, int tex_num );

    std::string	_filename;
  };

}


#endif

