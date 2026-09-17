/*
 *  ECAT data reader class
 */
#ifndef AIMS_IO_ECATR_H
#define AIMS_IO_ECATR_H

#include <aims/io/ecatheader.h>
#include <aims/data/data.h>

namespace aims
{

  template< class T > class EcatReader;


  template< class T > EcatReader< T >& 
  operator >> (EcatReader< T >& reader, AimsData< T >& thing);


  /** The class for Ecat data read operation.
   */
  template< class T >
  class EcatReader
  {
  public:
    EcatReader(const std::string& name) 
      : _name(name) ,_scale(1.0)
    { }
    virtual ~EcatReader() { }

    void read( AimsData< T >& thing, const carto::AllocatorContext & context, 
               carto::Object options );
    void frameRead( AimsData< T >& thing, 
                    const carto::AllocatorContext & context, int frame, 
                    int border=0 );

    /// Return a name without .v extension
    std::string removeExtension( const std::string& name );

    float getScale( ) { return _scale; };

    /// Input stream operator
    friend
    EcatReader< T >& operator >> <>(EcatReader< T >& reader, 
				    AimsData< T >& thing);

  private:
    std::string		_name;
    float		_scale;
  };


  template< class T > inline
  EcatReader< T >&
  operator >> ( EcatReader< T >& reader, AimsData< T >& thing )
  {
    reader.read( thing );
    return( reader );
  }

}

#endif
