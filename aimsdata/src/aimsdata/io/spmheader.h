
#ifndef AIMS_IO_SPMHEADER_H
#define AIMS_IO_SPMHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <string>

namespace aims
{

  /**	SPM Header class.
	Private attributes :

	Attribute name       type       Comment

	- minimum     	     int	???
	- maximum	     int	I don't know what's these for...
  */
  class AIMSDATA_API SpmHeader : public PythonHeader
  {
  public:
    SpmHeader( const std::string & name );
    SpmHeader( int dimx, int dimy, int dimz, int dimt, float sx, float sy, 
	       float sz, float st, const std::string & name );
    virtual ~SpmHeader();

    const std::string& name() const;
    void setName( const std::string & fname ) { _name = fname; }

    /// Get dimX... sizeT dimensions and sizes
    int dimX()  const;
    int dimY()  const;
    int dimZ()  const;
    int dimT()  const;
    float sizeX() const;
    float sizeY() const;
    float sizeZ() const;
    float sizeT() const;

    virtual std::string extension() const { return( ".img" ); }
    virtual std::set<std::string> extensions() const;
    bool read();
    bool write( bool writeMinf = true, bool allow4d = true );

    bool isNormalized() const;
    void setupWriteAttributes( const std::string & datatypecode );

    bool imageIsProbablyLabel() const;

  private:
    std::string _name;
  };

}


#endif
