/*  Support of MINC file format (from the Montreal Neurological Institute)
 *
 *  Olivier Colliot, MNI, Montreal
 *  May 2004
 *
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



