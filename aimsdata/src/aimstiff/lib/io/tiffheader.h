#ifndef AIMS_IO_TIFFHEADER_H
#define AIMS_IO_TIFFHEADER_H

#include <aims/data/sliceformatheader.h>
#include <aims/def/general.h>
#include <string>
#include <vector>

namespace aims
{

  /** The descriptor class of the .dim GIS header.
      The programmer can either read an existing header, or write a new one.
  */
  class TiffHeader : public SliceFormatHeader
  {
  public:
    TiffHeader( const std::string& name, const std::string& type="UNKNOWN",
		int x=1, int y=1, int z=1, int t=1,
		float sx=1, float sy=1, float sz=1, float st=1 ) :
      SliceFormatHeader( name, x, y, z, t, sx, sy, sz ,st ), 
      _type(type), _dimX(x), _dimY(y), 
      _dimZ(z), _dimT(t), _sizeX(sx), _sizeY(sy), _sizeZ(sz), _sizeT(st)
    { }
    virtual ~TiffHeader() { }

    int dimX() const { return _dimX; }
    int dimY() const { return _dimY; }
    int dimZ() const { return _dimZ; }
    int dimT() const { return _dimT; }

    float sizeX() const { return _sizeX; }
    float sizeY() const { return _sizeY; }
    float sizeZ() const { return _sizeZ; }
    float sizeT() const { return _sizeT; }

    /// Get type of items ("U08", "S08", "U16", "S16", ...)
    std::string dataType() const { return _type; }
    void setType( const std::string & t );
    /// Get the file name of the header
    std::string name() const { return _name; }
    std::vector<std::string> possibleDataTypes() const;

    virtual std::string extension() const;
    virtual std::set<std::string> extensions() const;

    void read();

  private:
    std::string _type;
    int   _dimX;
    int   _dimY;
    int   _dimZ;
    int   _dimT;
    float  _sizeX;
    float  _sizeY;
    float  _sizeZ;
    float  _sizeT;
  };

}

#endif

