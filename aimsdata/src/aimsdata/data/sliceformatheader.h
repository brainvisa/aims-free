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

#ifndef AIMS_DATA_SLICEFORMATHEADER_H
#define AIMS_DATA_SLICEFORMATHEADER_H

#include <aims/data/pheader.h>
#include <vector>

namespace aims
{

  /**	Header used for 2D image formats. It contains a mechanism to 
     retreive a list of slices / frames to build a 3D or 4D volume from 
     a files list from the .minf attributes
  */
  class SliceFormatHeader : public PythonHeader
  {
  public:
    SliceFormatHeader( const std::string & filename, int dimx = 1, 
                       int dimy = 1, int dimz = 1, int dimt = 1, float vsx = 1,
                       float vsy = 1, float vsz = 1, float vst = 1 );
    virtual ~SliceFormatHeader();

    virtual std::vector<std::string> inputFilenames();
    virtual std::vector<std::string> 
    outputFilenames() const;
    std::string inputNamepattern( unsigned & smin, unsigned & smax, 
                                  unsigned & tmin, unsigned & tmax ) const;
    std::string filename() const;

  protected:
    std::string	_name;
  };

}

#endif

