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

#ifndef VOLUMEFORMATWRITER_D_H
#define VOLUMEFORMATWRITER_D_H
//--- cartodata ----------------------------------------------------------------
#include <cartodata/io/volumeformatwriter.h>              // class declaration
#include <cartodata/volume/volume.h>                       // manipulate sizes
#include <cartodata/volume/volumeview.h>                   // manipulate sizes
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>        // function's argument
#include <soma-io/image/imagewriter.h>                // use of member functions
//--- cartobase ----------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
#include <cartobase/object/object.h>                         // header & options
#include <cartobase/object/property.h>                       // header & options
#include <cartobase/exception/ioexcept.h>                          // exceptions
#include <cartobase/config/verbose.h>                         // verbosity level
//--- system -------------------------------------------------------------------
#include <vector>
#include <iostream>
//------------------------------------------------------------------------------

namespace soma
{
////////////////////////////////////////////////////////////////////////////////
////                V O L U M E F O R M A T W R I T E R                     ////
////////////////////////////////////////////////////////////////////////////////

  //============================================================================
  //   C O N S T R U C T O R S
  //============================================================================
  template <typename T>
  VolumeFormatWriter<T>::~VolumeFormatWriter()
  {
  }
  
  //============================================================================
  //   W R I T E   M E T H O D S
  //============================================================================
  template <typename T>
  bool VolumeFormatWriter<T>::write( const carto::Volume<T> & obj, 
                                     carto::rc_ptr<DataSourceInfo> dsi,
                                     carto::Object options )
  {
    //=== memory mapping =======================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: checking for memory mapping..." 
                << std::endl;
    if( obj.allocatorContext().allocatorType() == AllocatorStrategy::ReadWriteMap )
      return true;
    
    //=== multiresolution level ================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: reading resolution level..." 
                << std::endl;
    int level = 0;
    if( options->hasProperty( "resolution_level" ) )
      options->getProperty( "resolution_level", level );
    
    //=== partial reading ======================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: checking for partial reading..." 
                << std::endl;
    bool partial = false;
    if( options->hasProperty( "partial_writing" ) )
      partial = true;
    if( carto::debugMessageLevel > 3 && partial ) {
      std::cout << "VOLUMEFORMATWRITER:: partial writing enabled" << std::endl;
    }
    
    std::vector<int> position( 4, 0 );
    std::vector<int> view( 4, 0 );
    std::vector<int> size( 4, 0 );
    
    //=== checking if obj is a view ============================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: checking if object is a view..." 
                << std::endl;
    const carto::VolumeView<T> *vv 
      = dynamic_cast<const carto::VolumeView<T> *>( &obj );
    carto::VolumeView<T> *p1vv;
    carto::Volume<T> *parent1 = 0;
    carto::Volume<T> *parent2 = 0;
    if( vv ) {
      parent1 = vv->refVolume().get();
      p1vv = dynamic_cast<carto::VolumeView<T> *>( parent1 );
      if( p1vv )
        parent2 = p1vv->refVolume().get();
    }
    if( carto::debugMessageLevel > 3 ) {
      std::cout << "VOLUMEFORMATWRITER:: object "
                << ( vv ? "is" : "isn't" ) << " a view and "
                << ( obj.allocatorContext().isAllocated() ? "is" : "isn't" )
                << " allocated." << std::endl;
      if( parent1 )
        std::cout << "VOLUMEFORMATWRITER:: parent1 exists and " 
                  << ( parent1->allocatorContext().isAllocated() ? "is" : "isn't" )
                  << " allocated." << std::endl;
      if( parent2 )
        std::cout << "VOLUMEFORMATWRITER:: parent2 exists and " 
                  << ( parent2->allocatorContext().isAllocated() ? "is" : "isn't" )
                  << " allocated." << std::endl;
    }

    //=== view size ============================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: reading view size..." 
                << std::endl;
    view[ 0 ] = obj.getSizeX();
    view[ 1 ] = obj.getSizeY();
    view[ 2 ] = obj.getSizeZ();
    view[ 3 ] = obj.getSizeT();

    //=== full volume size =====================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: reading full volume size and view position..." 
                << std::endl;
    if( parent1 && !parent1->allocatorContext().isAllocated() ) {
      size[ 0 ] = parent1->getSizeX();
      size[ 1 ] = parent1->getSizeY();
      size[ 2 ] = parent1->getSizeZ();
      size[ 3 ] = parent1->getSizeT();
      position[ 0 ] = vv->posInRefVolume()[ 0 ];
      position[ 1 ] = vv->posInRefVolume()[ 1 ];
      position[ 2 ] = vv->posInRefVolume()[ 2 ];
      position[ 3 ] = vv->posInRefVolume()[ 3 ];
    } else if( parent2 ) {
      size[ 0 ] = parent2->getSizeX();
      size[ 1 ] = parent2->getSizeY();
      size[ 2 ] = parent2->getSizeZ();
      size[ 3 ] = parent2->getSizeT();
      position[ 0 ] = vv->posInRefVolume()[ 0 ] + p1vv->posInRefVolume()[ 0 ];
      position[ 1 ] = vv->posInRefVolume()[ 1 ] + p1vv->posInRefVolume()[ 1 ];
      position[ 2 ] = vv->posInRefVolume()[ 2 ] + p1vv->posInRefVolume()[ 2 ];
      position[ 3 ] = vv->posInRefVolume()[ 3 ] + p1vv->posInRefVolume()[ 3 ];
    } else {
      size = view;
    }
    
    if( !partial ) {
      // we treat the view as a pure Volume
      size = view;
      position = std::vector<int>( 4, 0 );
    }
    
    if( carto::debugMessageLevel > 3 ) {
      std::cout << "VOLUMEFORMATWRITER:: Full volume size : ( "
                << size[0] << ", "
                << size[1] << ", "
                << size[2] << ", "
                << size[3] << " )"
                << std::endl;
      std::cout << "VOLUMEFORMATWRITER:: View size : ( "
                << view[0] << ", "
                << view[1] << ", "
                << view[2] << ", "
                << view[3] << " )"
                << std::endl;
      std::cout << "VOLUMEFORMATWRITER:: View position : ( "
                << position[0] << ", "
                << position[1] << ", "
                << position[2] << ", "
                << position[3] << " )"
                << std::endl;
    }
    
    //=== checking for borders =================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: checking for borders..." << std::endl;
    bool withborders = false;
    if( parent1 && parent1->allocatorContext().isAllocated() )
      withborders = true;
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: " 
                << ( withborders ? "with borders" : "without borders" ) 
                << std::endl;
    
    //=== header info ==========================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: setting header..." 
                << std::endl;
    if( !options )
      options = carto::Object::value( carto::PropertySet() );
    if( !vv && !obj.allocatorContext().isAllocated() )
      options->setProperty( "unallocated", true );
    if( !dsi->header() )
      dsi->header() = carto::Object::value( carto::PropertySet() );
    dsi->header()->setProperty( "sizeX", size[ 0 ] );
    dsi->header()->setProperty( "sizeY", size[ 1 ] );
    dsi->header()->setProperty( "sizeZ", size[ 2 ] );
    dsi->header()->setProperty( "sizeT", size[ 3 ] );
    try {
      const carto::PropertySet & ps = obj.header();
      dsi->header()->setProperty( "voxel_size", ps.getProperty( "voxel_size" ) );
    } catch( ... ) {
      std::vector<float> voxel_size( 4, 1. );
      dsi->header()->setProperty( "voxel_size", voxel_size );
    }

    
    //=== writing header & creating files ======================================
    // TODO how to test ?
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: writing header..." 
                << std::endl;
    *dsi = _imw->writeHeader( *dsi, options );
    
    //=== writing image ========================================================
    if( carto::debugMessageLevel > 3 )
      std::cout << "VOLUMEFORMATWRITER:: writing volume..." 
                << std::endl;
    if( vv || obj.allocatorContext().isAllocated() ) 
    {
      if( !withborders ) {
        _imw->write( (T*) &obj(0,0,0,0), *dsi, position, view, options );
      } else {
        int y, z, t;
        std::vector<int> posline ( position );
        std::vector<int> sizeline ( 4, 1 );
        sizeline[ 0 ] = view[ 0 ];
        for( t=0; t<view[3]; ++t )
          for( z=0; z<view[2]; ++z )
            for( y=0; y<view[1]; ++y ) {
              posline[ 1 ] = position[ 1 ] + y;
              posline[ 2 ] = position[ 2 ] + z;
              posline[ 3 ] = position[ 3 ] + t;
              _imw->write( (T*) &obj(0,y,z,t), *dsi, posline, 
                          sizeline, options );
            }
      }
    }
    // else we just needed to write the header and reserve file space
    // no image to write
    
    // we reset at 0 the ImageWriter's members (sizes, binary, ...) so that 
    // they are recomputed at the next writing.
    _imw->resetParams();
    return true;
  }
  
  template <typename T>
  void VolumeFormatWriter<T>::attach( carto::rc_ptr<ImageWriter<T> > imw )
  {
    _imw = imw;
  }
  
////////////////////////////////////////////////////////////////////////////////
////             V O L U M E R E F F O R M A T W R I T E R                  ////
////////////////////////////////////////////////////////////////////////////////

  template <typename T>
  VolumeRefFormatWriter<T>::~VolumeRefFormatWriter()
  {
  }
  
  template <typename T>
  bool VolumeRefFormatWriter<T>::write( const carto::VolumeRef<T> & obj, 
                                        carto::rc_ptr<DataSourceInfo> dsi,
                                        carto::Object options )
  {
    VolumeFormatWriter<T> vfw;
    vfw.attach( _imw );
    return vfw.write( *obj, dsi, options );
  }
  
  template <typename T>
  void VolumeRefFormatWriter<T>::attach( carto::rc_ptr<ImageWriter<T> > imw )
  {
    _imw = imw;
  }
  
}

#endif