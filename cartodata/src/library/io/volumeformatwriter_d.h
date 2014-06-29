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

#ifndef CARTODATA_IO_VOLUMEFORMATWRITER_D_H
#define CARTODATA_IO_VOLUMEFORMATWRITER_D_H
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatwriter.h>              // class declaration
#include <cartodata/volume/volume.h>                       // manipulate sizes
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>      // function's argument
#include <soma-io/image/imagewriter.h>              // use of member functions
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
#include <cartobase/object/object.h>                       // header & options
#include <cartobase/object/property.h>                     // header & options
#include <cartobase/exception/ioexcept.h>                        // exceptions
//--- system -----------------------------------------------------------------
#include <vector>
#include <iostream>
#include <string>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "VOLUMEFORMATWRITER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

namespace soma
{
//////////////////////////////////////////////////////////////////////////////
////               V O L U M E F O R M A T W R I T E R                    ////
//////////////////////////////////////////////////////////////////////////////

  //==========================================================================
  //   C O N S T R U C T O R S
  //==========================================================================
  template <typename T>
  VolumeFormatWriter<T>::~VolumeFormatWriter()
  {
  }

  //==========================================================================
  //   W R I T E   M E T H O D S
  //==========================================================================
  template <typename T>
  bool VolumeFormatWriter<T>::write( const carto::Volume<T> & obj,
                                     carto::rc_ptr<DataSourceInfo> dsi,
                                     carto::Object options )
  {
    //=== memory mapping =====================================================
    localMsg( "checking for memory mapping..." );
    if( obj.allocatorContext().allocatorType() == AllocatorStrategy::ReadWriteMap )
      return true;

    //=== multiresolution level ==============================================
    localMsg( "reading resolution level..." );
    int level = 0;
    if( options->hasProperty( "resolution_level" ) )
      options->getProperty( "resolution_level", level );
    localMsg( " -> level to write : " + carto::toString( level ) );

    //=== partial reading ====================================================
    localMsg( "checking for partial writing..." );
    bool partial = false;
    if( options->hasProperty( "partial_writing" ) )
      partial = true;
    if( partial )
      localMsg( " -> partial writing enabled." );

    std::vector<int> position( 4, 0 );
    std::vector<int> view( 4, 0 );
    std::vector<int> size( 4, 0 );

    //=== checking if obj is a view ==========================================
    localMsg( "checking if object is a view..." );
    carto::Volume<T> *parent1 = 0;
    carto::Volume<T> *parent2 = 0;
    parent1 = obj.refVolume().get();
    if( parent1 )
      parent2 = parent1->refVolume().get();
    localMsg( std::string("object ") + ( parent1 ? "is" : "isn't" ) + " a view and "
              + ( obj.allocatorContext().isAllocated() ? "is" : "isn't" )
              + " allocated." );
    if( parent1 )
      localMsg( std::string("parent1 exists and ")
                + ( parent1->allocatorContext().isAllocated() ? "is" : "isn't" )
                + " allocated." );
    if( parent2 )
      localMsg( std::string("parent2 exists and ")
                + ( parent2->allocatorContext().isAllocated() ? "is" : "isn't" )
                + " allocated." );

    //=== view size ==========================================================
    localMsg( "reading view size..." );
    view[ 0 ] = obj.getSizeX();
    view[ 1 ] = obj.getSizeY();
    view[ 2 ] = obj.getSizeZ();
    view[ 3 ] = obj.getSizeT();

    //=== full volume size ===================================================
    localMsg( "reading full volume size and view position..." );
    if( parent1 && !parent1->allocatorContext().isAllocated() ) {
      localMsg( " -> from parent1" )
      size[ 0 ] = parent1->getSizeX();
      size[ 1 ] = parent1->getSizeY();
      size[ 2 ] = parent1->getSizeZ();
      size[ 3 ] = parent1->getSizeT();
      position[ 0 ] = obj.posInRefVolume()[ 0 ];
      position[ 1 ] = obj.posInRefVolume()[ 1 ];
      position[ 2 ] = obj.posInRefVolume()[ 2 ];
      position[ 3 ] = obj.posInRefVolume()[ 3 ];
    } else if( parent2 ) {
      localMsg( " -> from parent2" )
      size[ 0 ] = parent2->getSizeX();
      size[ 1 ] = parent2->getSizeY();
      size[ 2 ] = parent2->getSizeZ();
      size[ 3 ] = parent2->getSizeT();
      position[ 0 ] = obj.posInRefVolume()[ 0 ] + parent1->posInRefVolume()[ 0 ];
      position[ 1 ] = obj.posInRefVolume()[ 1 ] + parent1->posInRefVolume()[ 1 ];
      position[ 2 ] = obj.posInRefVolume()[ 2 ] + parent1->posInRefVolume()[ 2 ];
      position[ 3 ] = obj.posInRefVolume()[ 3 ] + parent1->posInRefVolume()[ 3 ];
    } else {
      localMsg( " -> from self" )
      size = view;
    }

    if( !partial ) {
      // we treat the view as a pure Volume
      localMsg( " -> from self (no partial writing)" )
      size = view;
      position = std::vector<int>( 4, 0 );
    }

    localMsg( " -> Full volume size : ( "
              + carto::toString( size[0] ) + ", "
              + carto::toString( size[1] ) + ", "
              + carto::toString( size[2] ) + ", "
              + carto::toString( size[3] ) + " )" );
    localMsg( " -> View size : ( "
              + carto::toString( view[0] ) + ", "
              + carto::toString( view[1] ) + ", "
              + carto::toString( view[2] ) + ", "
              + carto::toString( view[3] ) + " )" );
    localMsg( " -> View position : ( "
              + carto::toString( position[0] ) + ", "
              + carto::toString( position[1] ) + ", "
              + carto::toString( position[2] ) + ", "
              + carto::toString( position[3] ) + " )" );

    //=== checking for borders ===============================================
    localMsg( "checking for borders..." );
    bool withborders = false;
    if( parent1 && parent1->allocatorContext().isAllocated() )
      withborders = true;
    localMsg( std::string(" -> ") + ( withborders ? "with borders" : "without borders" ) );

    //=== header info ========================================================
    localMsg( "setting header..." );
    if( !options )
      options = carto::Object::value( carto::PropertySet() );
    if( !parent1 && !obj.allocatorContext().isAllocated() )
      options->setProperty( "unallocated", true );
    if( !dsi->header() )
    {
      dsi->header() = carto::Object::value( carto::PropertySet() );
      dsi->header()->copyProperties(
        carto::Object::reference( obj.header() ) );
    }
    dsi->header()->setProperty( "sizeX", size[ 0 ] );
    dsi->header()->setProperty( "sizeY", size[ 1 ] );
    dsi->header()->setProperty( "sizeZ", size[ 2 ] );
    dsi->header()->setProperty( "sizeT", size[ 3 ] );
    if( !dsi->header()->hasProperty( "voxel_size" ) )
    {
      std::vector<float> voxel_size( 4, 1. );
      dsi->header()->setProperty( "voxel_size", voxel_size );
    }


    //=== use optional parameters for partial writing ========================
    if( partial ) {
      try {
        position[0] = (int) rint( options->getProperty( "ox" )->getScalar() );
//         localMsg( "override ox : " + carto::toString(position[0]) );
        std::cout << "override ox : " + carto::toString(position[0]) << std::endl;
      } catch( ... ) {}
      try {
        position[1] = (int) rint( options->getProperty( "oy" )->getScalar() );
//         localMsg( "override oy : " + carto::toString(position[1]) );
        std::cout << "override oy : " + carto::toString(position[1]) << std::endl;
      } catch( ... ) {}
      try {
        position[2] = (int) rint( options->getProperty( "oz" )->getScalar() );
//         localMsg( "override oz : " + carto::toString(position[2]) );
        std::cout << "override oz : " + carto::toString(position[2]) << std::endl;
      } catch( ... ) {}
      try {
        position[3] = (int) rint( options->getProperty( "ot" )->getScalar() );
//         localMsg( "override ot : " + carto::toString(position[3]) );
        std::cout << "override ot : " + carto::toString(position[3]) << std::endl;
      } catch( ... ) {}
    }

    //=== writing header & creating files ====================================
    localMsg( "writing header..." );
    *dsi = _imw->writeHeader( *dsi, options );

    //=== writing image ======================================================
    localMsg( "writing volume..." );
    if( parent1 || obj.allocatorContext().isAllocated() )
    {
      std::vector<long> strides(4);
      strides[0] = &obj(1,0,0,0) - &obj(0,0,0,0);
      strides[1] = &obj(0,1,0,0) - &obj(0,0,0,0);
      strides[2] = &obj(0,0,1,0) - &obj(0,0,0,0);
      strides[3] = &obj(0,0,0,1) - &obj(0,0,0,0);
//       if( !withborders ) {
        _imw->write( (T*) &obj(0,0,0,0), *dsi, position, view, strides,
                     options );
//       } else {
//         int y, z, t;
//         std::vector<int> posline ( position );
//         std::vector<int> sizeline ( 4, 1 );
//         sizeline[ 0 ] = view[ 0 ];
//         for( t=0; t<view[3]; ++t )
//           for( z=0; z<view[2]; ++z )
//             for( y=0; y<view[1]; ++y ) {
//               posline[ 1 ] = position[ 1 ] + y;
//               posline[ 2 ] = position[ 2 ] + z;
//               posline[ 3 ] = position[ 3 ] + t;
//               _imw->write( (T*) &obj(0,y,z,t), *dsi, posline,
//                           sizeline, options );
//             }
//       }
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

//////////////////////////////////////////////////////////////////////////////
////            V O L U M E R E F F O R M A T W R I T E R                 ////
//////////////////////////////////////////////////////////////////////////////

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

#undef localMsg
#endif
