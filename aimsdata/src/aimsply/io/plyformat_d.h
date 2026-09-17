#ifndef AIMS_IO_PLYFORMAT_D_H
#define AIMS_IO_PLYFORMAT_D_H

#include <aims/io/plyformat.h>
#include <aims/io/plyr.h>
#include <aims/io/plyw.h>

namespace aims
{

#ifdef __APPLE__
  // Bug in Apple compiler/linker (MacOS 10.3): this symbol is undefined
  template <>
  FileFormat<AimsSurfaceTriangle>::~FileFormat()
  {
  }
#endif

  template<long D, typename T>
  bool PlyFormat<D, T>::read( const std::string & filename, 
                              AimsTimeSurface<D,T> & vol, 
                              const carto::AllocatorContext & context, 
                              carto::Object options )
  {
    PlyReader<D, T>	r( filename );
    r.read( vol, context, options );
    return true;
  }

  template<long D, typename T>
  bool PlyFormat<D, T>::write( const std::string & filename, 
                               const AimsTimeSurface<D,T> & vol,
                               carto::Object options )
  {
    bool ascii = false;
    try
    {
      if( !options.isNull() )
      {
        carto::Object aso = options->getProperty( "ascii" );
        if( !aso.isNull() )
          ascii = (bool) aso->getScalar();
      }
    }
    catch( ... )
    {
    }

    try
    {
      PlyWriter<D, T>	r( filename, ascii );
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

