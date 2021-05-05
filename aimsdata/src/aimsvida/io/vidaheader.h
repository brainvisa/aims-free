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
