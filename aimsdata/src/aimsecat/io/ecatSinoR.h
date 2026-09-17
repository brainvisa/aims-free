
#ifndef AIMS_IO_ECATSINOR_H
#define AIMS_IO_ECATSINOR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/sino/sino.h>
#include <aims/io/datatypecode.h>
#include <aims/data/data.h>
#include <vector>


namespace aims
{

  template<class T> class EcatSinoReader
  {
  public:
    EcatSinoReader( const std::string & name ) : _name( name ) {}
    ~EcatSinoReader() {}

    void read( Sinogram<T> & thing, int bedDataOrFrame );
    void read( std::vector < Sinogram<T> > & thing );

    /// Return a name without .v extension
    std::string removeExtension( const std::string& name );

    /// set input file mode
    void setMode( FileOpenMode mode );

  private:
    std::string		_name;
  };

  template <class T>
  inline EcatSinoReader<T> & 
  operator >> ( EcatSinoReader<T> & reader, 
		std::vector < Sinogram<T> > & thing )
  {
    reader.read( thing );
    return reader;
  }
}

#endif
