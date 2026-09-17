#ifndef AIMS_IO_FDFFORMAT_D_H
#define AIMS_IO_FDFFORMAT_D_H

#include <aims/io/fdfformat.h>
#include <aims/io/fdfR.h>

namespace aims
{

  template<class T>
  bool FdfFormat<T>::read( const std::string & filename, AimsData<T> & vol,
                             const carto::AllocatorContext & context, 
                             carto::Object options )
  {
    FdfReader<T>	r( filename );
    r.read( vol, context, options );
    return( true );
  }

}

#endif
