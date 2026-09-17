/*
 *  Gis header class
 */
#ifndef AIMS_IO_GENESISHEADER_H
#define AIMS_IO_GENESISHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <string>


namespace aims
{

  /** GE Genesis format header
  */
  class AIMSDATA_API GenesisHeader : public PythonHeader
  {
  public:
    GenesisHeader( const std::string& name, int dimx = 0, int dimy = 0, 
		   int dimz = 0, int dimt = 0, float sx = 1, float sy = 1, 
		   float sz = 1, float st = 1, 
		   const std::string & dtype = "S16" );
    virtual ~GenesisHeader();

    int dimX() const { return _dimX; }
    int dimY() const { return _dimY; }
    int dimZ() const { return _dimZ; }
    int dimT() const { return _dimT; }
    float sizeX() const { return _sizeX; }
    float sizeY() const { return _sizeY; }
    float sizeZ() const { return _sizeZ; }
    float sizeT() const { return _sizeT; }

    std::string dataType() const { return _type; }
    std::string name() const { return _name; }
    int compressCode() const { return( _compress ); }
    int dataOffset() const { return( _data_offset ); }
    bool byteSwapping() const { return( _bswap ); }

    virtual std::string extension() const { return( "" ); }
    virtual std::set<std::string> extensions() const;

    void read();

  private:
    std::string	_name;
    std::string	_type;
    int	_dimX;
    int	_dimY;
    int	_dimZ;
    int	_dimT;
    float	_sizeX;
    float	_sizeY;
    float	_sizeZ;
    float	_sizeT;
    int		_compress;
    int		_data_offset;
    bool	_bswap;
  };

}


#endif
