/*  Support of MINC file format (from the Montreal Neurological Institute)
 *
 *  Olivier Colliot, MNI, Montreal
 *  May 2004
 *
 *
 *
 *  Minc header class
 *  
 */

#ifndef AIMS_IO_MINCHEADER_H
#define AIMS_IO_MINCHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <aims/def/general.h>
#include <string>

//MINC I/O support
extern "C" {
#include <volume_io.h>
}

namespace aims
{

  /** The descriptor class of the MINC header.
  */
  class AIMSDATA_API MincHeader : public PythonHeader
  {
  public:
    /**@name Constructor and Destructor*/
    //@{

    /** 

    The constructor has 2 roles : either to give the name of an existing
        header, or to inform the class of the various parameters.
        @param name file name
        @param type string code of the type of the items [default="UNKNOWN"]
        @param x dimension of X axis [default=1]
        @param y dimension of Y axis [default=1]
        @param z dimension of Z axis [default=1]
        @param t dimension of T axis [default=1]
        @param sx resolution of X axis [default=1.0]
        @param sy resolution of Y axis [default=1.0]
        @param sz resolution of Z axis [default=1.0]
        @param st resolution of T axis [default=1.0]
    */
    inline MincHeader(const std::string& name, 
		     const std::string& type="UNKNOWN",
		     int x=1, int y=1, int z=1, int t=1,
		     float sx=1, float sy=1, float sz=1, float st=1) :
      PythonHeader(), 
      _name(name), _type(type), _dimX(x), _dimY(y), 
      _dimZ(z), _dimT(t), _sizeX(sx), _sizeY(sy), _sizeZ(sz), _sizeT(st)
    {}

    /// Destructor does nothing
    virtual ~MincHeader() { }
    //@}

    /**@name Methods*/
    //@{
    /// Get dimension in X axis
    int dimX() const { return _dimX; }
    /// Get dimension in Y axis
    int dimY() const { return _dimY; }
    /// Get dimension in Z axis
    int dimZ() const { return _dimZ; }
    /// Get dimension in T axis
    int dimT() const { return _dimT; }

    /// Get resolution in X axis
    float sizeX() const { return _sizeX; }
    /// Get resolution in Y axis
    float sizeY() const { return _sizeY; }
    /// Get resolution in Z axis
    float sizeZ() const { return _sizeZ; }
    /// Get resolution in T axis
    float sizeT() const { return _sizeT; }

    /// Get type of items ("U8", "S8", "FLOAT", "S16", ...)
    std::string dataType() const { return _type; }
    std::vector<std::string> possibleDataTypes() const { return _pdt; }
    /// Get the file name of the header
    std::string name() const { return _name; }

 
    /// Get the open mode ("binar" or "ascii")
      //    std::string openMode() const { return "binar"; }

    virtual std::string extension() const { return( ".mnc" ); }
    virtual std::set<std::string> extensions() const;

    /// Read the header
    void read();
    /// Write the header
    void write();
    //@}

    int writeMincAttribute(carto::Syntax &sx, int mincid, 
                           std::string minc_var, nc_type minc_var_type, 
                           std::string minc_var_parent, std::string minc_att, 
                           std::string aims_att_name);

    int writeMincHistory(int mincid);

    /// mute output function
    static void my_empty_print_error(char *message);

  private:
    /**@name Data*/
    //@{
    /// File name
    std::string _name;
    /// Type of items
    std::string _type;
    std::vector<std::string>	_pdt;
    /// Dimension in X axis
    int   _dimX;
    /// Dimension in Y axis
    int   _dimY;
    /// Dimension in Z axis
    int   _dimZ;
    /// Dimension in T axis
    int   _dimT;
    /// Resolution in X axis
    float  _sizeX;
    /// Resolution in Y axis
    float  _sizeY;
    /// Resolution in Z axis
    float  _sizeZ;
    /// Resolution in T axis
    float  _sizeT;

    int readMincAttribute(carto::Syntax &sx, int mincid, 
                          std::string minc_var, std::string minc_att, 
                          std::string aims_att_name);

    int readMincHistory(int mincid);
    //@}
  };

}


#endif
