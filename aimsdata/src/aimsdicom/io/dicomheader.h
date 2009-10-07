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

#ifndef AIMS_IO_DICOMHEADER_H
#define AIMS_IO_DICOMHEADER_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <string>
#include <map>


namespace aims
{

  class AIMSDATA_API DicomHeader : public PythonHeader
  {
  public:
    class FileElement
    {
    public:

      FileElement( int i=-1, double l=0.0, const std::string n="" ) 
	: _instance( i ), _loc( l ), _name( n )  { }
      virtual ~FileElement()  { }

      int instance() const { return _instance; }
      std::string name() const { return _name; }
      double location() const { return _loc; }

    private:

      int _instance;
      double _loc;
      std::string _name;
    };

    DicomHeader( const std::string& name );
    virtual ~DicomHeader();

    std::string name() const;
    const std::map< int, FileElement >& slices() const { return _slices; }
    bool reverseZ() const { return _reverseZ; }

    /// Get dimX... sizeT dimensions and sizes
    int dimX()  const;
    int dimY()  const;
    int dimZ()  const;
    int dimT()  const;
    float sizeX() const;
    float sizeY() const;
    float sizeZ() const;
    float sizeT() const;

    int read();

  private:

    std::map< int, FileElement > _slices;

    std::string _name;
    bool _reverseZ;

    int readFirst();
    FileElement readNext( const std::string& );
  };


}

#endif

