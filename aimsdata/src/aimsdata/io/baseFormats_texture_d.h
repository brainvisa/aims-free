#ifndef AIMS_IO_BASEFORMATS_TEXTURE_D_H
#define AIMS_IO_BASEFORMATS_TEXTURE_D_H

#include <aims/io/baseFormats_texture.h>
#include <aims/io/texR.h>
#include <aims/io/texW.h>


namespace aims
{

  template<class T>
  bool TexFormat<T>::read( const std::string & filename, TimeTexture<T> & obj, 
                           const carto::AllocatorContext & /*context*/, 
                           carto::Object options )
  {
    int	frame = -1;
    options->getProperty( "frame", frame );
    TexReader<T>	r( filename );
    r.read( obj, frame );
    return( true );
  }

  template<class T>
  bool TexFormat<T>::write( const std::string & filename, 
			    const TimeTexture<T> & obj, carto::Object options )
  {
    TexWriter<T>	w( filename );
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
    w.write( obj, ascii );
    return( true );
  }

}


#endif
