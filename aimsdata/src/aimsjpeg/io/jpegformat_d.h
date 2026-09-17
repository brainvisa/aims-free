#ifndef AIMS_IO_JPEGFORMAT_D_H
#define AIMS_IO_JPEGFORMAT_D_H

#include <aims/io/jpegformat.h>
#include <aims/io/jpegR.h>
#include <aims/io/jpegW.h>

namespace aims
{

  template<class T>
  bool JpegFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    JpegReader<T>	r( filename );
    r.read( vol, context, options );
    return( true );
  }

  template<class T>
  bool JpegFormat<T>::write( const std::string & filename, 
			     const AimsData<T> & vol, carto::Object )
  {
    try
      {
	JpegWriter<T>	r( filename );
	r.write( vol );
      }
    catch( std::exception & e )
      {
	return( false );
      }

    return( true );
  }

}

#endif

