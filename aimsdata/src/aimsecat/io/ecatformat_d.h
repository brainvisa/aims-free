#ifndef AIMS_IO_ECATFORMAT_D_H
#define AIMS_IO_ECATFORMAT_D_H

#include <aims/io/ecatformat.h>
#include <aims/io/ecatR.h>
#include <aims/io/ecatW.h>

namespace aims
{

  template<class T>
  bool EcatFormat<T>::read( const std::string & filename, AimsData<T> & vol, 
                            const carto::AllocatorContext & context, 
                            carto::Object options )
  {
    EcatReader<T>	r( filename );
    r.read( vol, context, options );
    return( true );
  }

  template<class T>
  bool EcatFormat<T>::write( const std::string & filename, 
			     const AimsData<T> & vol, carto::Object )
  {
    try
      {
	EcatWriter	r( filename );
	r.write( vol );
      }
    catch( std::exception & e )
      {
	//cerr << e.what() << endl;
	return false;
      }

    return true;
  }

}

#endif
