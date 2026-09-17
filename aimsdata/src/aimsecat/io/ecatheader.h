/*
 *  ECAT image header
 */
#ifndef AIMS_IO_ECATHEADER_H
#define AIMS_IO_ECATHEADER_H

#include <aims/data/pheader.h>
#include <string>

#define _typeINIT 1
#define ECAT_QUANT 100
#define ECAT_LABEL 200


namespace aims
{


/** The descriptor class of the Ecat header.
    The programmer can only read an existing header. It cannot write a new one.
    Ecat data are signed 16 bits 4D objects.
*/
  class EcatHeader : public aims::PythonHeader
  { 
  public:
    EcatHeader(const std::string& name );
    EcatHeader( int dimx, int dimy, int dimz, int dimt, float sx, float sy, 
		float sz, float st, int type, const std::string & name );
    virtual ~EcatHeader();

    /// Get type of items (ECAT_QUANT or ECAT_LABEL)
    int itemType() const { return _type; }

    /// Get the file name of the header
    std::string name() const { return _name; }
  
    virtual std::string extension() const { return( ".v" ); }
    virtual std::set<std::string> extensions() const;

    /// Get dimX... sizeT dimensions and sizes
    int dimX()  const;
    int dimY()  const;
    int dimZ()  const;
    int dimT()  const;
    float sizeX() const;
    float sizeY() const;
    float sizeZ() const;
    float sizeT() const;
    std::string imageUnit() const ;

    /// Read the header
    void read();
    /// Guess the image for labels or quantitative data.
    bool imageIsProbablyLabel() const;
    /// Write the header (not yet implemented)
    void write();
    
  private:
    std::string _name;
    /// Type of items
    int  _type;
  };


}


#endif
