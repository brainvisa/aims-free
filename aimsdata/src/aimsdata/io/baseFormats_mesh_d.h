/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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
