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
