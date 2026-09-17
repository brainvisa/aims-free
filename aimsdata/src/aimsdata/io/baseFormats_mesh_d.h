/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_MESH_D_H
#define AIMS_IO_BASEFORMATS_MESH_D_H

#include <aims/io/baseFormats_mesh.h>
#include <aims/io/meshR.h>
#include <aims/io/meshW.h>
#include <aims/io/povW.h>
#include <aims/io/wavefrontmeshW.h>
#include <aims/io/wavefrontmeshR.h>


namespace aims
{

  //	MESH

  template<int D, typename T>
  MeshFormat<D,T>::~MeshFormat()
  {
  }


  template<int D, class T>
  bool MeshFormat<D,T>::read( const std::string & filename, 
                              AimsTimeSurface<D,T> & obj, 
                              const carto::AllocatorContext & /*context*/, 
                              carto::Object options )
  {
    int	frame = -1;
    options->getProperty( "frame", frame );
    MeshReader<D,T>	r( filename );
    r.read( obj, frame );
    return( true );
  }

  template<int D, class T> bool 
  MeshFormat<D,T>::write( const std::string & filename, 
                          const AimsTimeSurface<D,T> & obj,
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
    MeshWriter<D,T>	r( filename, ascii );
    r.write( obj );
    return( true );
  }

  //	POV

  template<int D>
  PovFormat<D>::~PovFormat()
  {
  }


  template<int D>
  bool PovFormat<D>::read( const std::string &, 
                           AimsTimeSurface<D,Void> &, 
                           const carto::AllocatorContext &, carto::Object )
  {
    return( false );
  }

  template<int D>
  bool PovFormat<D>::write( const std::string & filename, 
                            const AimsTimeSurface<D,Void> & obj, carto::Object )
  {
    PovWriter<D> r( filename );
    r.write( obj );
    return( true );
  }


  //	WAVEFRONT

  template<int D, typename T>
  WavefrontMeshFormat<D, T>::~WavefrontMeshFormat()
  {
  }


  template<int D, typename T>
  bool WavefrontMeshFormat<D, T>::read( const std::string & filename,
                           AimsTimeSurface<D,T> & obj,
                           const carto::AllocatorContext & context,
                           carto::Object options )
  {
    WavefrontMeshReader<D, T>	r( filename );
    r.read( obj, context, options );
    return true;
  }

  template<int D, typename T>
  bool WavefrontMeshFormat<D, T>::write( const std::string & filename,
                            const AimsTimeSurface<D, T> & obj,
                            carto::Object options )
  {
    WavefrontMeshWriter<D, T> r( filename );
    r.write( obj, options );
    return true;
  }

}


#endif
