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
 *  Data writer class
 */
#ifndef AIMS_IO_MOTIONW_H
#define AIMS_IO_MOTIONW_H

#include <fstream>
#include <string>
#include <aims/io/defaultItemW.h>
#include <aims/resampling/motion.h>

namespace aims{
  class MotionWriter;
  

  MotionWriter&
  operator << (MotionWriter& writer,
	       const Motion& thing);

/**	<b>--- OBSOLETE ---</b>

    The class for motion write operation.
*/
class __attribute__((__deprecated__("OBSOLETE"))) MotionWriter
{ public:
    /**@name Constructor and Destructor*/
    //@{
    /** The programmer should give the file name to the writer.
        @param name reference to the file name
        @param openmode open mode [default="binar"]
    */
    MotionWriter(const std::string& name, int mode=std::ios::out ) 
      : _name( name ), _mode( mode )  { }
    /// Destructor does nothing special
    virtual ~MotionWriter() { }
    //@}

    /**@name Methods*/
    //@{
    /** Write the motion with "name" file name to disk
        @param thing const reference to the data to write
    */
    void write(const Motion& thing);

  /// Output stream operator
  friend
  MotionWriter& 
  operator <<( MotionWriter& writer,
	       const Motion& thing);
    //@}

  private:
    /**@name Data*/
    //@{
    /// File name
    std::string   _name;
    //@}
    //@{
    /// Open mode
    int          _mode;
    //@}
};


inline MotionWriter&
operator << (MotionWriter& writer,
             const Motion& thing)
{ 
  writer.write(thing);
  return(writer);
}

}

#endif










