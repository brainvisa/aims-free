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

    virtual std::string extension() const { return( ".tiff" ); }
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

