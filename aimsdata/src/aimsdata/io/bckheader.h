/*
 *  Bucket header class
 */
#ifndef AIMS_IO_BCKHEADER_H
#define AIMS_IO_BCKHEADER_H


#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <aims/def/general.h>


namespace aims
{

/** Descriptor class for the .bck BUCKET file format header.
*/
  class AIMSDATA_API BckHeader : public PythonHeader
  {
  public:
    BckHeader( const std::string& name, const std::string & type = "UNKNOWN", 
	       float sizex = 1, float sizey = 1, float sizez = 1, 
	       float sizet = 1, uint byteorder = AIMS_MAGIC_NUMBER, 
	       const std::string & openmode = "binar" );
    virtual ~BckHeader() { }

    std::string name() const { return _name; }
    std::string dataType() const { return _type; }

    virtual int dimX() const { return( 0 ); }
    virtual int dimY() const { return( 0 ); }
    virtual int dimZ() const { return( 0 ); }
    virtual int dimT() const { return( _ntime ); }
    virtual float sizeX() const;
    virtual float sizeY() const;
    virtual float sizeZ() const;
    virtual float sizeT() const;

    uint byteOrder() const { return _byteOrder; }
    std::string openMode() const { return _openMode; }

    std::string filename() const;
    virtual std::string extension() const { return( ".bck" ); }
    virtual std::set<std::string> extensions() const;

    void read( uint32_t* offset = 0 );
    void write();

  private:
    std::string	_name;
    std::string	_type;
    int		_ntime;
    uint	_byteOrder;
    std::string	_openMode;
  };

}


#endif
