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


#ifndef AIMS_IO_NIFTIHEADER_H
#define AIMS_IO_NIFTIHEADER_H

#include <aims/io/niftilib/nifti1_io.h>
#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <string>

namespace aims
{

  /**	NIFTI Header class.
  */
  class AIMSDATA_API NiftiHeader : public PythonHeader
  {
  public:
    NiftiHeader( const std::string & name );
    NiftiHeader( int dimx, int dimy, int dimz, int dimt, float sx, float sy, 
                 float sz, float st, const std::string & name );
    virtual ~NiftiHeader();
    void freeNiftiStruct();

    const std::string& name() const;
    void setName( const std::string & fname ) { _name = fname; }

    virtual std::string extension() const;
	virtual std::string basename() const;
    virtual std::set<std::string> extensions() const;
	
	
	nifti_image* niftiNim() { return _nim; }

    int dimX()  const;
    int dimY()  const;
    int dimZ()  const;
    int dimT()  const;
    float sizeX() const;
    float sizeY() const;
    float sizeZ() const;
    float sizeT() const;

    bool read();
    bool write( bool writeminf = true );
	
	bool fillNim( bool allow4d = true );

  private:
    std::string _name;
	nifti_image *_nim;
  };

}

#endif
