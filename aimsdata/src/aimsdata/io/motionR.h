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

#ifndef AIMS_IO_MOTIONR_H
#define AIMS_IO_MOTIONR_H

#include <aims/config/aimsdata_config.h>
#include <fstream>
#include <string>

#include <aims/resampling/motion.h> 

class MotionReader;

/** Class for Motion reading operation.
*/
class AIMSDATA_API MotionReader
{ 
public:
  /**@name Constructor and Destructor*/
  //@{
  /** The programmer should give the file name to the reader.
      @param name referes to the file name
 
  */
  MotionReader( const std::string& name );
  /// Destructor does nothing special
  virtual ~MotionReader();
  //@}

  /**@name Methods*/
  //@{
  /// set input file name
  inline void setFileName( const std::string &fileName );

  /** Read the Motion with "name" file name from disk

  */
  void read( Motion& thing );

  MotionReader& operator >> ( Motion& );

private:

  /**@name Data*/
  //@{
  /// File name
  std::string   _name;
  //@}
};


inline void MotionReader::setFileName( const std::string &fileName )
{
  _name = fileName;
}


#endif
