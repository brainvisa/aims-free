/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


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
  ASSERT( _is );

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
