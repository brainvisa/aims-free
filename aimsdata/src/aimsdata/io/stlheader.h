/*
 *  Finder class
 */
#ifndef AIMS_IO_STLHEADER_H
#define AIMS_IO_STLHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>


namespace carto
{
  class DataSource;
}


namespace aims
{

  class STLHeader : public PythonHeader
  {
  public:
    STLHeader( const std::string & filename );
    virtual ~STLHeader();

    /** Reads the header, and if \c offset is not null, sets the file offset
        to the data field */
    virtual bool read( size_t *offset = 0 );
    virtual std::string openMode() const;
    virtual bool byteSwapping() const;
    std::string filename() const;
    virtual std::string extension() const { return( ".stl" ); }
    virtual std::set<std::string> extensions() const;

  private:
    void readMesh( bool stopWhenIdentified );
    void readMeshAscii( bool stopWhenIdentified, carto::DataSource & ds );
    void readMeshBinary( bool stopWhenIdentified, carto::DataSource & ds );

    std::string	_filename;
  };

}


#endif

