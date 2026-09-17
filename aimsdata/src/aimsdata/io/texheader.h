/*
 *  Texture header class
 */
#ifndef AIMS_IO_TEXHEADER_H
#define AIMS_IO_TEXHEADER_H


#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>
#include <aims/data/pheader.h>


namespace aims
{

  class AIMSDATA_API TexHeader : public PythonHeader
  {
  public:
    TexHeader( const std::string & name, const std::string & type = "", 
	       size_t size = 0, bool ascii = false );
    virtual ~TexHeader();

    std::string name() const;
    std::string dataType() const;

    uint byteOrder() const;
    virtual std::string openMode() const;
    virtual std::string filename() const;
    virtual std::string extension() const { return std::string(".tex"); }
    virtual std::set<std::string> extensions() const;
    virtual bool byteSwapping() const;

    virtual void read( size_t* offset = 0 );
    virtual void write();

    /// useless compatibility functions (Header must be modified!)
    virtual int dimX () const { return 0; }
    virtual int dimY () const { return 0; }
    virtual int dimZ () const { return 0; }
    virtual int dimT () const;
    virtual float sizeX () const { return 1.0; }
    virtual float sizeY () const { return 1.0; }
    virtual float sizeZ () const { return 1.0; }
    virtual float sizeT () const { return 1.0; }

  private:
    std::string _name;
    std::string _type;
    size_t _size;
    uint  _byteOrder;
    std::string _openMode;
  };


  inline
  std::string TexHeader::dataType() const
  {
    return _type;
  }

  inline
  uint TexHeader::byteOrder() const
  {
    return _byteOrder;
  }

  inline
  std::string TexHeader::openMode() const
  {
    return _openMode;
  }

}


#endif
