#ifndef AIMS_IO_TIFFFORMAT_D_H
#define AIMS_IO_TIFFFORMAT_D_H

#include <aims/io/tiffformat.h>
#include <aims/io/tiffR.h>
#include <aims/io/tiffW.h>

namespace aims
{

  template<class T>
  bool TiffFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    TiffReader<T>	r( filename );
    r.read( vol, context, options );
    return true;
  }

  template<class T>
  bool TiffFormat<T>::write( const std::string & filename, 
			     const AimsData<T> & vol, carto::Object )
  {
    try
      {
	TiffWriter<T>	r( filename );
	r.write( vol );
      }
    catch( std::exception & e )
      {
	return false;
      }

    return true;
  }

}

#endif
