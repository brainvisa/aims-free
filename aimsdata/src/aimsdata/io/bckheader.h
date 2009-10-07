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
