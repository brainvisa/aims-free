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

#ifndef AIMS_IO_MINCW_H
#define AIMS_IO_MINCW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/data/data.h>
#include <cartobase/exception/file.h>
#include <aims/io/datatypecode.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

//MINC I/O support
extern "C" {
#include <volume_io.h>
}

namespace aims
{

  template<class T>
  class AIMSDATA_API MincWriter
  {
  public:
  /**
     A class for writing MINC files (from the Montreal Neurological Institute)
     It is still under development.
  */

    MincWriter( const std::string& name, bool ascii = false, 
	       ItemWriter<T>* ir = 0 ) 
      : _name( name ), _itemw( ir ), _binmode( !ascii ) {}
    ~MincWriter() { delete _itemw; }

    bool write( const AimsData<T>& thing );
    /// set input file mode
    void setMode( bool ascii );
    bool asciiMode() const { return( !_binmode ); }
    void setItemWriter( ItemWriter<T>* ir )
      { delete _itemw; _itemw = ir; }
    std::string name() { return( _name ); }

  private:
    std::string		_name;
    ItemWriter<T>	*_itemw;
    bool		_binmode;
  };


  template <class T>
  inline AIMSDATA_API MincWriter<T> & 
  operator << ( MincWriter<T> & writer, const AimsData<T> & thing )
  {
    writer.write( thing );
    return writer;
  }

  template <class T>
  inline
  void MincWriter<T>::setMode( bool ascii )
  {
    _binmode = !ascii;
  }






}
#endif



