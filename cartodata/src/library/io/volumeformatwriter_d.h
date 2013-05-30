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
//--- system -------------------------------------------------------------------
#include <vector>
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
    if( obj.allocatorContext().allocatorType() == AllocatorStrategy::ReadWriteMap )
      return true;
    
    //=== multiresolution level ================================================
    int level = 0;
    if( options->hasProperty( "resolution_level" ) )
      options->getProperty( "resolution_level", level );
    
    //=== partial reading ======================================================
    bool partial = false;
    if( options->hasProperty( "partial_writing" ) )
      partial = true;
    
    std::vector<int> position( 4, 0 );
    std::vector<int> view( 4, 0 );
    std::vector<int> size( 4, 0 );
    
    //=== checking if obj is a view ============================================
    const carto::VolumeView<T> *vv 
        = dynamic_cast<const carto::VolumeView<T> *>( &obj );
    if( vv ) {
      // obj is a VolumeView
      size[ 0 ] = vv->refVolume()->getSizeX();
      size[ 1 ] = vv->refVolume()->getSizeY();
      size[ 2 ] = vv->refVolume()->getSizeZ();
      size[ 3 ] = vv->refVolume()->getSizeT();
      view[ 0 ] = vv->getSizeX();
      view[ 1 ] = vv->getSizeY();
      view[ 2 ] = vv->getSizeZ();
      view[ 3 ] = vv->getSizeT();
      position[ 0 ] = vv->posInRefVolume()[ 0 ];
      position[ 1 ] = vv->posInRefVolume()[ 1 ];
      position[ 2 ] = vv->posInRefVolume()[ 2 ];
      position[ 3 ] = vv->posInRefVolume()[ 3 ];
    } else {
      // obj is a pure Volume
      size[ 0 ] = obj.getSizeX();
      size[ 1 ] = obj.getSizeY();
      size[ 2 ] = obj.getSizeZ();
      size[ 3 ] = obj.getSizeT();
      view = size;
    }
    
    if( !partial ) {
      // we treat the view as a pure Volume
      size = view;
      position = std::vector<int>( 4, 0 );
    }
    
    //=== header info ==========================================================
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
    const carto::PropertySet & ps = obj.header();
    dsi->header()->setProperty( "voxel_size", ps.getProperty( "voxel_size" ) );

    
    //=== writing header & creating files ======================================
    // TODO how to test ?
    *dsi = _imw->writeHeader( *dsi, options );
    
    //=== writing image ========================================================
    if( obj.allocatorContext().isAllocated() ) {
      // case without borders
      _imw->write( (T*) &obj(0,0,0,0), *dsi, position, view, level, options );
    } else if( vv ) {
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
                         sizeline, level, options );
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