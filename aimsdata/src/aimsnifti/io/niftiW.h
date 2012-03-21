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

#ifndef AIMS_IO_NIFTIW_H
#define AIMS_IO_NIFTIW_H

#include <aims/data/data.h>
#include <string>


namespace aims
{
  template <class T> class NiftiWriter;

  template <class T>
  NiftiWriter<T>& operator << ( NiftiWriter<T>& writer, const AimsData<T>& thing );

  template <class T>
  class NiftiWriter
  {
  public:
    NiftiWriter( const std::string& name ) 
      : _name( name ) { }
    virtual ~NiftiWriter() { }

    /** Write the data with "name" file name to disk
        @param thing const reference to the data to write
    */
    void write( const AimsData<T>& thing,
                carto::Object options = carto::none() );

    /// Return a name without  extension
    inline std::string removeExtension( const std::string& name );

    /// Output stream operator
    friend
    NiftiWriter<T>& 
    operator << <> (NiftiWriter<T>& writer, const AimsData<T>& thing);


  private:
    std::ofstream _os;
    std::string   _name;
  };
  
  template <class T> inline
  NiftiWriter<T>& operator << ( NiftiWriter<T>& writer, const AimsData<T>& thing )
  {
    writer.write(thing);
    return writer;
  }
}

#endif





