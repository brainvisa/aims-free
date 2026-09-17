#ifndef AIMS_IO_VIDAFORMAT_D_H
#define AIMS_IO_VIDAFORMAT_D_H

#include <aims/io/vidaformat.h>
#include <aims/io/vidaR.h>
#include <aims/io/vidaW.h>
#include <aims/io/spmR.h>
#include <aims/io/spmW.h>

namespace aims
{

  template<class T>
  bool VidaFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    VidaReader<T>	r( filename );
    r.read( vol, context, options );
    return true;
  }

  template<class T>
  bool VidaFormat<T>::write( const std::string & filename, 
			     const AimsData<T> & vol, carto::Object )
  {
    try
      {
	VidaWriter	r( filename );
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
