#ifndef AIMS_IO_VIDAHEADER_H
#define AIMS_IO_VIDAHEADER_H

#include <aims/data/pheader.h>
#include <vidaIO/kernel/vidaio.h>
#include <string>


namespace aims
{


  /** The descriptor class of the VIDA header.
      The programmer can only read an existing header. It cannot write a new 
      one.
      VIDA data are signed 16 bits 4D objects.
  */
  class VidaHeader : public PythonHeader
  {
  public:

    VidaHeader( const std::string& name );
    VidaHeader( int dimx, int dimy, int dimz, int dimt, float sx, 
		float sy, float sz, float st, const std::string& name = "" );
    /// Destructor does nothing
    virtual ~VidaHeader();

    /// Get type of items (FX_SHORT_FX or FX_SHORT_FL)
    int itemType() const;
  
    /// Get the file name of the header
    std::string name() const;

    virtual std::string extension() const { return( ".vimg" ); }
    virtual std::set<std::string> extensions() const;

    int dimX()  const;
    int dimY()  const;
    int dimZ()  const;
    int dimT()  const;
    float sizeX() const;
    float sizeY() const;
    float sizeZ() const;
    float sizeT() const;
    std::vector<float> getVoxelSize() const;
 
    /// Read the header
    void read();
    bool imageIsProbablyLabel() const;
    /// Write the header
    void write();


  private:
    std::string _name;
    int _type;
  };


}


#endif
