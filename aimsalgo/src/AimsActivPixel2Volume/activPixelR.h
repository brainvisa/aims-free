
#ifndef _AIMS_ACTIVPIXELR_H_
#define _AIMS_ACTIVPIXELR_H_

#include <string>
#include <fstream>
#include <aims/def/assert.h>
#include <aims/io/defaultItemR.h>
#include "activPixel.h"

class AimsActivPixelReader;

AimsActivPixelReader& 
operator >> (AimsActivPixelReader& reader, std::vector<Point3df>& thing);


/** The class for Activ oixel read operation.
*/
class AimsActivPixelReader
{ 
  public:
    /**@name Constructor and Destructor*/
    //@{
    /** The programmer should give the file name to the reader.
        @param name reference to the file name
    */
    AimsActivPixelReader( const std::string& name ) : _name( name ) { }
    /// Destructor does nothing special
    virtual ~AimsActivPixelReader() { }
    //@}

    /**@name Methods*/
    //@{
    /** Read a single ActivPixel item
        @param item reference to the item to read
    */
    void read( AimsActivPixel& thing );

    /// Input stream operator
    friend AimsActivPixelReader& 
      operator >> ( AimsActivPixelReader& reader, AimsActivPixel& thing );
    //@}

  private:
    /**@name Data*/
    //@{
    /// Input stream
    std::ifstream _is;
    /// File name
    std::string _name;
    /// Get the code associated to the value type
    inline std::string code() const { return "ACTIVPIXEL"; }
    //@}
};


inline
void AimsActivPixelReader::read( AimsActivPixel& thing )
{
  char buf[256];
  float x=0.0, y=0.0, z=0.0;

  _is.open( (char *)_name.c_str(), std::ios::in );
  ASSERT( _is.good() );

  aims::DefaultAsciiItemReader< float > item;

  std::string tosearch1 = "Voxel size in mm (x,y,z):";
  do
    {
      _is.getline( buf, 256 );
    }
  while ( std::string( buf ) != tosearch1 );

  item.read( _is, x );
  item.read( _is, y );
  item.read( _is, z );
  Point3df tmp( x, y, z );
  thing.voxelSize() = tmp;

  std::string tosearch2 = "Activated pixels in mm (x,y,z):";
  do
    {
      _is.getline( buf, 256 );
    }
  while ( std::string( buf ) != tosearch2 );

  item.read( _is, x );
  item.read( _is, y );
  item.read( _is, z );
  tmp[0] = x;
  tmp[1] = y;
  tmp[2] = z;
  while ( !_is.eof() )
    {
      thing.activPoints().push_back( Point3df( tmp ) );
      item.read( _is, x );
      item.read( _is, y );
      item.read( _is, z );
      tmp[0] = x;
      tmp[1] = y;
      tmp[2] = z;
    }

  _is.close();
}


inline AimsActivPixelReader&
operator >> ( AimsActivPixelReader& reader, AimsActivPixel& thing )
{
  reader.read( thing );
  return reader;
}

#endif
