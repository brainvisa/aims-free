/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_BUCKET_D_H
#define AIMS_IO_BASEFORMATS_BUCKET_D_H

#include <aims/io/baseFormats_bucket.h>
#include <aims/io/bckR.h>
#include <aims/io/bckW.h>
#include <aims/io/bckMapR.h>
#include <aims/io/bckMapW.h>


namespace aims
{

  template<class T>
  bool BckFormat<T>::read( const std::string & filename, AimsBucket<T> & obj, 
			   const carto::AllocatorContext & /*context*/, 
                           carto::Object options )
  {
    BckReader<T>	r( filename );
    int			frame = -1;
    options->getProperty( "frame", frame );
    r.read( obj, frame );
    return( true );
  }

  template<class T>
  bool BckFormat<T>::write( const std::string & filename, 
			    const AimsBucket<T> & obj, carto::Object options
                          )
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
    BckWriter<T>	w( filename, ascii );
    w.write( obj );
    return( true );
  }

  //	BCK format read as BucketMap

  template<class T>
  bool BckMapFormat<T>::read( const std::string & filename, 
                              BucketMap<T> & obj, 
                              const carto::AllocatorContext & /*context*/, 
                              carto::Object options )
  {
    BckMapReader<T>	r( filename );
    int			frame = -1;
    options->getProperty( "frame", frame );
    r.read( obj, frame );
    return( true );
  }

  template<class T>
  bool BckMapFormat<T>::write( const std::string & filename, 
			       const BucketMap<T> & obj, carto::Object options
                             )
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
    BckMapWriter<T>	w( filename, ascii );
    w.write( obj );
    return( true );
  }

}


#endif
