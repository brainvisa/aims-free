#ifndef AIMS_IO_READERASOBJECT_H
#define AIMS_IO_READERASOBJECT_H

#include <aims/io/process.h>
#include <aims/io/reader.h>

namespace aims
{

  class ReaderAsObject : public Process
  {
  public:
    ReaderAsObject( const std::string& filename = "" );
    virtual ~ReaderAsObject();
    carto::Object read( int border = 0, const std::string* format = 0, 
                        int frame = -1 );

    /// callback functions
    template <typename T> static 
    bool readAsObject( Process &, const std::string &, Finder & );

  private:
    std::string		_filename;
    carto::Object	_result;
    int			_border;
    const std::string	*_format;
    int			_frame;
  };


  template <typename T>
  inline 
  bool ReaderAsObject::readAsObject( Process & p, const std::string & fname, 
                                     Finder & )
  {
    ReaderAsObject	& ord = (ReaderAsObject &) p;
    ord._result = carto::Object::value( T() );
    Reader<T>	r( fname );
    r.read( ord._result->carto::GenericObject::value<T>(), ord._border, 
            ord._format, ord._frame );
    return true;
  }

}

#endif

